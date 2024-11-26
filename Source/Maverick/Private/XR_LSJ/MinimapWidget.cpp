// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/MinimapWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/OperatorPawn.h"
#include "InputActionValue.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/TriggerBox.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "LDG/Soldier.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/LineBatchComponent.h"
#include "LDG/SoldierAIController.h"
#include "LDG/TankBase.h"
#include "LDG/TankAIController.h"
FReply UMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 클릭된 위치를 위젯 공간에서 좌표로 얻기
    FVector2D LocalClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 클릭된 위치로 플레이어 이동
        MovePlayerToMapClick(LocalClickPosition);   
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
       if (AOperatorPawn* PlayerPawn = Cast<AOperatorPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
       {
            FVector2D ClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
            //미니맵 위치를 월드 위치로 변환
            FVector WorldPosition = ConvertingMinimapToLocation(ClickPosition);
            //카메라 높이에서 카메라 회전값을 넣은 LineTrace 
            //DrawDebugLine(GetWorld(),WorldPosition,WorldPosition+FVector(0,0,10000),FColor::Red,true,1000);
            PlayerPawn->OnMouseRightMinimap(WorldPosition);

            
            // 현재 화면 크기 가져오기
            FVector2D CurrentViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
            // 현재 화면 따라 비율 변경 
            //CurrentViewportSize.X = 1852 / CurrentViewportSize.X;
			//CurrentViewportSize.Y = 1073 / CurrentViewportSize.Y;
			CurrentViewportSize.X = 1920 / CurrentViewportSize.X;
			CurrentViewportSize.Y = 1080 / CurrentViewportSize.Y;
            //경로 찾기
            //거리비교
            float minDistance = 1000000;
            FVector StartLocation;
            TArray<APawn*> Units;
            for (ASoldier* Unit : PlayerPawn->GetSelectedUnits())
			{   
               if(nullptr==Unit)
                  continue;
               float TargetDistance = FVector::Distance(Unit->GetActorLocation(),WorldPosition);
               if (TargetDistance < minDistance)
               {
                   minDistance = TargetDistance;
                   StartLocation = Unit->GetActorLocation();
               }
               Units.Add(Unit);
            }
            for (ATankBase* Unit : PlayerPawn->GetSelectedTanks())
			{   
               if(nullptr==Unit)
                  continue;
               float TargetDistance = FVector::Distance(Unit->GetActorLocation(),WorldPosition);
               if (TargetDistance < minDistance)
               {
                   minDistance = TargetDistance;
                   StartLocation = Unit->GetActorLocation();
                   WorldPosition.Z = 132.0f;
               }
               Units.Add(Unit);
            }
            
            TArray<FVector> Path;
            TArray<TPair<FVector2D, FVector2D>> PathLines;
			FindPath(StartLocation, WorldPosition, Path);
			for (int32 PathIdx = 0; PathIdx < Path.Num() - 1; PathIdx++)
			{
				FVector2D MinimapPosition = ConvertingLocationToMinimap(Path[PathIdx]);
				FVector2D NextMinimapPosition = ConvertingLocationToMinimap(Path[PathIdx + 1]);
                //MinimapPosition*=CurrentViewportSize;
                MinimapPosition.X*=CurrentViewportSize.X;
                MinimapPosition.Y*=CurrentViewportSize.Y;
                //NextMinimapPosition*=CurrentViewportSize;
                NextMinimapPosition.X*=(CurrentViewportSize.X);
                NextMinimapPosition.Y*=CurrentViewportSize.Y;
                //미니맵에 경로 그리기
                //UE_LOG(LogTemp,Error,TEXT(" fff %s %s"),*(MinimapPosition * CurrentViewportSize).ToString(), *(NextMinimapPosition * CurrentViewportSize).ToString());
				//AddRuntimeLine(MinimapPosition * CurrentViewportSize, NextMinimapPosition * CurrentViewportSize);
                PathLines.Add(TPair<FVector2D, FVector2D>(MinimapPosition, NextMinimapPosition));
			}
            if(Unit_MinimapPath.Contains(Units))
            Unit_MinimapPath[Units]=PathLines;
            else
            Unit_MinimapPath.Add(Units,PathLines);
       }
    }



    return FReply::Handled();
}
void UMinimapWidget::MovePlayerToMapClick(const FVector2D& ClickPosition)
{
    if(ClickPosition.X<35||ClickPosition.X>370||ClickPosition.Y>1055||ClickPosition.Y<490)
        return;

    // 플레이어 캐릭터를 가져와서 해당 위치로 이동 명령
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
    if (PlayerPawn)
    {
         // 오프셋을 월드 위치로 변환 
        float NormalX = FMath::Lerp(0.f,1.f,(ClickPosition.X-29.294f)/(374.085-29.294));
        float NormalY = FMath::Lerp(1.f,0.f,(ClickPosition.Y-470.f)/(1054.f-470.f)); //462.746 //1035.151
        //UE_LOG(LogTemp,Warning,TEXT("ClickPosition.Y %f"),ClickPosition.Y);
        float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
        float WorldPositionY = FMath::Lerp(-23280.0,18780.0,NormalY); //1054.779541 //506.431274
        FVector WorldPosition;
        WorldPosition.X = WorldPositionX;
        WorldPosition.Y = WorldPositionY + 2000.f;
       //UE_LOG(LogTemp,Error,TEXT("%f %f %s"), NormalX,NormalY,*ClickPosition.ToString());
        WorldPosition.Z = PlayerPawn->GetActorLocation().Z; // 높이값 고정 또는 특정 레벨의 높이로 설정
        PlayerPawn->SetActorLocation(WorldPosition);
    }
}

void UMinimapWidget::CreateWarningUI(FVector Location)
{
	if (UUserWidget* WarningUI = CreateWidget<UUserWidget>(GetWorld(), WarningUIClass))
	{
		FVector2D Position = ConvertingLocationToMinimap(Location);
		WarningUI->SetPositionInViewport(Position);
		WarningUI->AddToViewport();

        FTimerHandle CreateWarningUIHandle1;
	    GetWorld()->GetTimerManager().SetTimer(CreateWarningUIHandle1,[&,Position]()
	    {
		    if (UUserWidget* WarningUI = CreateWidget<UUserWidget>(GetWorld(), WarningUIClass))
		    {
			    WarningUI->SetPositionInViewport(Position);
			    WarningUI->AddToViewport();
		    }
	    },0.3f,false);
        FTimerHandle CreateWarningUIHandle2;
	    GetWorld()->GetTimerManager().SetTimer(CreateWarningUIHandle2,[&,Position]()
	    {
		    if (UUserWidget* WarningUI = CreateWidget<UUserWidget>(GetWorld(), WarningUIClass))
		    {
			    WarningUI->SetPositionInViewport(Position);
			    WarningUI->AddToViewport();
		    }
	    },0.5f,false);
	}
}

int32 UMinimapWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Create a Paint Context
    FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

    // Draw a line
    for (auto& pUnit_Unit_MinimapPath : Unit_MinimapPath)
    {
        const TArray<APawn*>& KeyArray = pUnit_Unit_MinimapPath.Key;
        if (0 >= KeyArray.Num())
        {
            continue;
        }
            
        for (auto& Position : pUnit_Unit_MinimapPath.Value)
        {
            UWidgetBlueprintLibrary::DrawLine(Context, Position.Key, Position.Value, FLinearColor::White, true, 2.0f);
        }
    }

    return LayerId; // Return the current layer ID
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    //미니맵에 이동 루트 그리기
    for (auto& pUnit_Unit_MinimapPath : Unit_MinimapPath)
    {
        
        int32 DieUnitCount = 0;
        TArray<APawn*>& KeyArray = pUnit_Unit_MinimapPath.Key;
        //UE_LOG(LogTemp,Warning,TEXT("KeyArray  %d"),KeyArray.Num());
        for (int32 UnitCount = KeyArray.Num() - 1; UnitCount>=0; UnitCount--)
        {
            if (nullptr == KeyArray[UnitCount] || KeyArray[UnitCount]->GetController() == nullptr ||  KeyArray[UnitCount]->IsActorBeingDestroyed())
            {
                DieUnitCount++;
				continue;
            }
            ASoldierAIController* SoldierController = Cast<ASoldierAIController>(KeyArray[UnitCount]->GetController());
			if (SoldierController && (SoldierController->IsDead() || SoldierController->GetCurrentState() == EState::Die || SoldierController->GetCurrentState() == EState::Attack))
			{
                DieUnitCount++;
				continue;
			}
            ATankAIController* TankController = Cast<ATankAIController>(KeyArray[UnitCount]->GetController());
			if (TankController && (TankController->CurrentState == ETankState::Die || TankController->CurrentState == ETankState::Attack))
			{
                DieUnitCount++; 
				continue;
			}
           
            // 현재 화면 크기 가져오기
            FVector2D CurrentViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
            // 현재 화면 따라 비율 변경 
            CurrentViewportSize.X = 1955 / CurrentViewportSize.X;
			CurrentViewportSize.Y = 1065 / CurrentViewportSize.Y;
            //CurrentViewportSize.X = 1852 / CurrentViewportSize.X;
		    //CurrentViewportSize.Y = 1073 / CurrentViewportSize.Y;
            FVector2D UnitPosition = ConvertingLocationToMinimap(pUnit_Unit_MinimapPath.Key[UnitCount]->GetActorLocation());
            UnitPosition*=CurrentViewportSize;
            if (pUnit_Unit_MinimapPath.Value.Num()>0)
            {
                if (FVector2D::Distance(UnitPosition, pUnit_Unit_MinimapPath.Value[0].Value) < 20.0f)
                {
                    pUnit_Unit_MinimapPath.Value.RemoveAt(0);
                    Invalidate(EInvalidateWidgetReason::Paint);
                }
                else 
                {
                    pUnit_Unit_MinimapPath.Value[0].Key=UnitPosition;
                    Invalidate(EInvalidateWidgetReason::Paint);
                }
            }
            break;
        }
        if(DieUnitCount>=pUnit_Unit_MinimapPath.Key.Num() || pUnit_Unit_MinimapPath.Value.Num()<=0)
        {
            pUnit_Unit_MinimapPath.Value.Empty();
			Invalidate(EInvalidateWidgetReason::Paint);
            Remove_MinimapPath.Add(pUnit_Unit_MinimapPath.Key);
            continue;
        }
    }

    for (const TArray<APawn*>& Key : Remove_MinimapPath)
	{
		Unit_MinimapPath.Remove(Key);
	}
    Remove_MinimapPath.Empty();
}

void UMinimapWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UMinimapWidget::NativeDestruct()
{
    Super::NativeDestruct();
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UMinimapWidget::AddRuntimeLine(const FVector2D& Start, const FVector2D& End)
{
    RuntimeLines.Add(TPair<FVector2D, FVector2D>(Start, End));
    Invalidate(EInvalidateWidgetReason::Paint);
}
FVector UMinimapWidget::ConvertingMinimapToLocation(FVector2D Position)
{
    //LandScape 를 가져와서 변환하자
    // 오프셋을 월드 위치로 변환 
    float NormalX = FMath::Lerp(0.f,1.f,(Position.X-29.294f)/(374.085-29.294));
    float NormalY = FMath::Lerp(1.f,0.f,(Position.Y-470.f)/(1054.f-470.f)); //462.746 //1035.151
    float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
    float WorldPositionY = FMath::Lerp(-23280.0,18780.0,NormalY); //1054.779541 //506.431274
    FVector WorldPosition;
    WorldPosition.X = WorldPositionX;
    WorldPosition.Y = WorldPositionY + 4000.f;
    WorldPosition.Z = 150.f; // 높이값 고정 또는 특정 레벨의 높이로 설정
    return WorldPosition;
}
FVector2D UMinimapWidget::ConvertingLocationToMinimap(FVector Location)
{
    //기준 좌표
    const FVector2D OriginLocation = FVector2D(-12641.0,-22269);
    //월드 크기
    const FVector2D MiniMapWorldSize = FVector2D(12900.0,22000.0);
    // 현재 위치를 Landscape의 로컬 좌표로 변환
    const float NormalizedX = (Location.X - OriginLocation.X) / (MiniMapWorldSize.X*2.0f);
    const float NormalizedY = (Location.Y - OriginLocation.Y) / ((MiniMapWorldSize.Y*2.0f-500.f));
    
    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Minimap->Slot);
    if (CanvasSlot)
    {
        //해상도에 따라 크기 가져오기
        const FGeometry& Geometry = Minimap->GetCachedGeometry();
        FVector2D Size = Geometry.GetAbsoluteSize();
        //미니맵 {0,0} 위치를 위젯좌표로 변환
		FVector2D Position = FVector2D(CanvasSlot->GetAlignment().X*Size.X/2 + CanvasSlot->GetAlignment().X*5.f,CanvasSlot->GetAlignment().Y*Size.Y/2 - CanvasSlot->GetAlignment().Y*100.f);
        //월드위치를 미니맵 위치로 변환
        double NormalX = FMath::Lerp(Position.X + Size.X ,Position.X ,NormalizedX);
        double NormalY = FMath::Lerp(Position.Y-Size.Y,Position.Y,NormalizedY);

        return FVector2D(NormalX,NormalY);
    }
    //UE_LOG(LogTemp,Warning,TEXT("Location.X %f OriginLocation.X %f  NormalizedY  %f"),(Location.X - OriginLocation.X), MiniMapWorldSize.X);
    //UE_LOG(LogTemp,Warning,TEXT("Location.X - OriginLocation.X %f.NormalizedY  %f"),(Location.Y - OriginLocation.Y),MiniMapWorldSize.Y);
    return FVector2D(0,0);
}

void UMinimapWidget::FindPath(const FVector& StartLocation,const FVector& TargetLocation, TArray<FVector>& ArrayLocation)
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if(nullptr == NavSystem)
        return;
    UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation);
    if (NavPath && NavPath->IsValid())
    {
        ArrayLocation = NavPath->PathPoints;
    }
}