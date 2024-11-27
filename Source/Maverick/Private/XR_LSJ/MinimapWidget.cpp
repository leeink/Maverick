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
FReply UMinimapWidget::NativeOnMouseButtonDoubleClick( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        CreatePathLine(InGeometry,InMouseEvent);
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UMinimapWidget::CreatePathLine(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (AOperatorPawn* PlayerPawn = Cast<AOperatorPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
       {
            FVector2D ClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
            //�̴ϸ� ��ġ�� ���� ��ġ�� ��ȯ
            FVector WorldPosition = ConvertingMinimapToLocation(ClickPosition);
            //ī�޶� ���̿��� ī�޶� ȸ������ ���� LineTrace 
            //DrawDebugLine(GetWorld(),WorldPosition,WorldPosition+FVector(0,0,10000),FColor::Red,true,1000);
            PlayerPawn->OnMouseRightMinimap(WorldPosition);

            
            // ���� ȭ�� ũ�� ��������
            FVector2D CurrentViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
            // ���� ȭ�� ���� ���� ���� 
            //CurrentViewportSize.X = 1852 / CurrentViewportSize.X;
			//CurrentViewportSize.Y = 1073 / CurrentViewportSize.Y;
			CurrentViewportSize.X = 1920 / CurrentViewportSize.X;
			CurrentViewportSize.Y = 1080 / CurrentViewportSize.Y;
            //��� ã��
            //�Ÿ���
            float minDistance = 1000000;
            FVector StartLocation;
            TArray<APawn*> Units;
            int32 NextKey = 1;
            for (auto& Pair : Num_MinimapPath)
            {
                if(Pair.Key == NextKey)
                    NextKey++;
                else
                    break;
            }
            for (ASoldier* Unit : PlayerPawn->GetSelectedUnits())
			{   
               if(nullptr==Unit)
                  continue;
               Unit->SetMinimapPathKey(NextKey);
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
                Unit->SetMinimapPathKey(NextKey);
               float TargetDistance = FVector::Distance(Unit->GetActorLocation(),WorldPosition);
               if (TargetDistance < minDistance)
               {
                   minDistance = TargetDistance;
                   StartLocation = Unit->GetActorLocation();
                   WorldPosition.Z = 132.0f;
               }
               Units.Add(Unit);
            }
            if(Units.Num()<=0)
				return;
			TArray<FVector> Path;
            TArray<TPair<FVector2D, FVector2D>> PathLines;
			FindPath(StartLocation, WorldPosition, Path);
			for (int32 PathIdx = 0; PathIdx < Path.Num() - 1; PathIdx++)
			{
				FVector2D MinimapPosition = ConvertingLocationToMinimap(Path[PathIdx]);
				FVector2D NextMinimapPosition = ConvertingLocationToMinimap(Path[PathIdx + 1]);

                MinimapPosition.X*=CurrentViewportSize.X;
                MinimapPosition.Y*=CurrentViewportSize.Y;
    
                NextMinimapPosition.X*=(CurrentViewportSize.X);
                NextMinimapPosition.Y*=CurrentViewportSize.Y;
                //�̴ϸʿ� ��� �׸���
                PathLines.Add(TPair<FVector2D, FVector2D>(MinimapPosition, NextMinimapPosition));
			}
            Num_MinimapPath.Add(NextKey,PathLines);
            Num_Units.Add(NextKey,Units);
    }
}
FReply UMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
  
    // Ŭ���� ��ġ�� ���� �������� ��ǥ�� ���
    FVector2D LocalClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // Ŭ���� ��ġ�� �÷��̾� �̵�
        MovePlayerToMapClick(LocalClickPosition);   

    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        CreatePathLine(InGeometry,InMouseEvent);
    }



     return FReply::Handled();
}
void UMinimapWidget::MovePlayerToMapClick(const FVector2D& ClickPosition)
{
    if(ClickPosition.X<35||ClickPosition.X>370||ClickPosition.Y>1055||ClickPosition.Y<490)
        return;

    // �÷��̾� ĳ���͸� �����ͼ� �ش� ��ġ�� �̵� ���
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
    if (PlayerPawn)
    {
         // �������� ���� ��ġ�� ��ȯ 
        float NormalX = FMath::Lerp(0.f,1.f,(ClickPosition.X-29.294f)/(374.085-29.294));
        float NormalY = FMath::Lerp(1.f,0.f,(ClickPosition.Y-470.f)/(1054.f-470.f)); //462.746 //1035.151
        //UE_LOG(LogTemp,Warning,TEXT("ClickPosition.Y %f"),ClickPosition.Y);
        float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
        float WorldPositionY = FMath::Lerp(-23280.0,18780.0,NormalY); //1054.779541 //506.431274
        FVector WorldPosition;
        WorldPosition.X = WorldPositionX;
        WorldPosition.Y = WorldPositionY + 2000.f;
       //UE_LOG(LogTemp,Error,TEXT("%f %f %s"), NormalX,NormalY,*ClickPosition.ToString());
        WorldPosition.Z = PlayerPawn->GetActorLocation().Z; // ���̰� ���� �Ǵ� Ư�� ������ ���̷� ����
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
    for (auto& pNum_MinimapPath : Num_MinimapPath)
    {
        for (auto& Position : pNum_MinimapPath.Value)
        {
            UWidgetBlueprintLibrary::DrawLine(Context, Position.Key, Position.Value, FLinearColor::White, true, 2.0f);
        }
    }
    /*
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
    */
    return LayerId; // Return the current layer ID
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    //�̴ϸʿ� �̵� ��Ʈ �׸���
    for (auto& pNum_MinimapPath : Num_MinimapPath)
    {
        int32 DieUnitCount = 0;
        APawn* FurthestUnit = nullptr;
        float MinDistance = 0.f;
        for (APawn* Unit : Num_Units[pNum_MinimapPath.Key])
        {   
            //�̵����� ������ �׾����� üũ�ϱ�
            if (nullptr == Unit)
            {
                DieUnitCount++;
				continue;
            }
            if (Unit->GetController() == nullptr)
            {
                DieUnitCount++;
				continue;
            }
            if (Unit->IsActorBeingDestroyed())
            {
                DieUnitCount++;
				continue;
            }
 
            if (ASoldier* SoldierUnit = Cast<ASoldier>(Unit))
            {
                if(SoldierUnit->GetMinimapPathKey()!=pNum_MinimapPath.Key)
              	{
                    DieUnitCount++;
				    continue;
			    }
            }
            ASoldierAIController* SoldierController = Cast<ASoldierAIController>(Unit->GetController());
			if (SoldierController && (SoldierController->IsDead() || SoldierController->GetCurrentState() == EState::Die || SoldierController->GetCurrentState() == EState::Attack))
			{
                DieUnitCount++;
				continue;
			}
            if (ATankBase* TankUnit = Cast<ATankBase>(Unit))
            {
                if (TankUnit->GetMinimapPathKey() != pNum_MinimapPath.Key)
              	{
                    DieUnitCount++;
				    continue;
			    }
            }
            ATankAIController* TankController = Cast<ATankAIController>(Unit->GetController());
			if (TankController && (TankController->CurrentState == ETankState::Die || TankController->CurrentState == ETankState::Attack))
			{
                DieUnitCount++; 
				continue;
			}
            if(pNum_MinimapPath.Value.Num()<=0)
                continue;
            FVector2D UnitPosition = ConvertingLocationToMinimap(Unit->GetActorLocation());
            float TargetDistance = FVector2D::Distance(UnitPosition, pNum_MinimapPath.Value.Last().Value);
            if (MinDistance < TargetDistance)
            {
                MinDistance = TargetDistance;
                FurthestUnit = Unit;
            }
            break;
        }
        if (Num_Units[pNum_MinimapPath.Key].Num() <= DieUnitCount || nullptr==FurthestUnit)
        {
            RemoveMinimapPath.Add(pNum_MinimapPath.Key);
            continue;
        }
        
        // ���� ȭ�� ũ�� ��������
        FVector2D CurrentViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
        // ���� ȭ�� ���� ���� ���� 
        CurrentViewportSize.X = 1955 / CurrentViewportSize.X;
		CurrentViewportSize.Y = 1065 / CurrentViewportSize.Y;
        //CurrentViewportSize.X = 1852 / CurrentViewportSize.X;
		//CurrentViewportSize.Y = 1073 / CurrentViewportSize.Y;
        FVector2D UnitPosition = ConvertingLocationToMinimap(FurthestUnit->GetActorLocation());
        UnitPosition*=CurrentViewportSize;
        if (pNum_MinimapPath.Value.Num()>0)
        {
            if (FVector2D::Distance(UnitPosition, pNum_MinimapPath.Value[0].Value) < 20.0f)
            {
                pNum_MinimapPath.Value.RemoveAt(0);
                Invalidate(EInvalidateWidgetReason::Paint);
            }
            else 
            {
                pNum_MinimapPath.Value[0].Key=UnitPosition;
                Invalidate(EInvalidateWidgetReason::Paint);
            }
        }
        else
        {
            RemoveMinimapPath.Add(pNum_MinimapPath.Key);
        }
    }
    for (const int32& RemoveIndex : RemoveMinimapPath)
	{
		Num_MinimapPath.Remove(RemoveIndex);
        Num_Units.Remove(RemoveIndex);
	}
    RemoveMinimapPath.Empty();
    ////////////////////////////////////////////////////
    /*
    for (auto& pUnit_Unit_MinimapPath : Unit_MinimapPath)
    {
        int32 DieUnitCount = 0;
        TArray<APawn*>& KeyArray = pUnit_Unit_MinimapPath.Key;
        //UE_LOG(LogTemp,Warning,TEXT("KeyArray  %d"),KeyArray.Num());
        for (int32 UnitCount = KeyArray.Num() - 1; UnitCount>=0; UnitCount--)
        {
            if (nullptr == KeyArray[UnitCount])
            {
                DieUnitCount++;
				continue;
            }
            if (KeyArray[UnitCount]->GetController() == nullptr)
            {
                DieUnitCount++;
				continue;
            }
            if (KeyArray[UnitCount]->IsActorBeingDestroyed())
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
           
            // ���� ȭ�� ũ�� ��������
            FVector2D CurrentViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
            // ���� ȭ�� ���� ���� ���� 
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
    */
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
    //LandScape �� �����ͼ� ��ȯ����
    // �������� ���� ��ġ�� ��ȯ 
    float NormalX = FMath::Lerp(0.f,1.f,FMath::Clamp(Position.X-27.f,0,374.f-27.f)/(374.f-27.f)); //27.000000 //373.621613
    float NormalY = FMath::Lerp(1.f,0.f,FMath::Clamp(Position.Y-480.f,0,1060.f-480)/(1060.f-480.f)); //462.746 //1035.151 //1060~480 1 0

    float WorldPositionX = FMath::Lerp(21650-10481.0,-10481.0,NormalX);
    float WorldPositionY = FMath::Lerp(-19279,39591-19279,NormalY); //1054.779541 //506.431274
    FVector WorldPosition;
    WorldPosition.X = WorldPositionX;
    WorldPosition.Y = WorldPositionY;
    WorldPosition.Z = 150.f; // ���̰� ���� �Ǵ� Ư�� ������ ���̷� ����

    return WorldPosition;
    ////LandScape �� �����ͼ� ��ȯ����
    //// �������� ���� ��ġ�� ��ȯ 
    //float NormalX = FMath::Lerp(0.f,1.f,(Position.X-29.294f)/(374.085-29.294));
    //float NormalY = FMath::Lerp(1.f,0.f,(Position.Y-470.f)/(1054.f-470.f)); //462.746 //1035.151
    //float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
    //float WorldPositionY = FMath::Lerp(-23280.0,18780.0,NormalY); //1054.779541 //506.431274
    //FVector WorldPosition;
    //WorldPosition.X = WorldPositionX;
    //WorldPosition.Y = WorldPositionY + 4000.f;
    //WorldPosition.Z = 150.f; // ���̰� ���� �Ǵ� Ư�� ������ ���̷� ����
    //return WorldPosition;
}
FVector2D UMinimapWidget::ConvertingLocationToMinimap(FVector Location)
{
    //���� ��ǥ
    const FVector2D OriginLocation = FVector2D(-12641.0,-22269);
    //���� ũ��
    const FVector2D MiniMapWorldSize = FVector2D(12900.0,22000.0);
    // ���� ��ġ�� Landscape�� ���� ��ǥ�� ��ȯ
    const float NormalizedX = (Location.X - OriginLocation.X) / (MiniMapWorldSize.X*2.0f);
    const float NormalizedY = (Location.Y - OriginLocation.Y) / ((MiniMapWorldSize.Y*2.0f-500.f));
    
    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Minimap->Slot);
    if (CanvasSlot)
    {
        //�ػ󵵿� ���� ũ�� ��������
        const FGeometry& Geometry = Minimap->GetCachedGeometry();
        FVector2D Size = Geometry.GetAbsoluteSize();
        //�̴ϸ� {0,0} ��ġ�� ������ǥ�� ��ȯ
		FVector2D Position = FVector2D(CanvasSlot->GetAlignment().X*Size.X/2 + CanvasSlot->GetAlignment().X*5.f,CanvasSlot->GetAlignment().Y*Size.Y/2 - CanvasSlot->GetAlignment().Y*100.f);
        //������ġ�� �̴ϸ� ��ġ�� ��ȯ
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