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
FReply UMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 클릭된 위치를 위젯 공간에서 좌표로 얻기
        FVector2D LocalClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

        // 클릭된 위치로 플레이어 이동
        MovePlayerToMapClick(LocalClickPosition);

        if (AOperatorPawn* PlayerPawn = Cast<AOperatorPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
        {
            
            for (ASoldier* Unit : PlayerPawn->GetSelectedUnits())
            {
                TArray<FVector> Path;
                //FindPath(Unit->GetActorLocation(),LocalClickPosition,Path);
                TArray<FVector2D> Path2D;
                for (FVector Location : Path)
                {
                    Path2D.Add(ConvertingLocationToMinimap(Location));
                }
                //UWidgetBlueprintLibrary::DrawLines(*CustomContext, Path2D, FLinearColor::White, true, 2.0f);
            }
            
        }
        
           
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
       if (AOperatorPawn* PlayerPawn = Cast<AOperatorPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
       {
            FVector2D ClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
            //LandScape 를 가져와서 변환하자
            // 오프셋을 월드 위치로 변환 
            float NormalX = FMath::Lerp(0.f,1.f,(ClickPosition.X-29.294f)/(374.085-29.294));
            float NormalY = FMath::Lerp(1.f,0.f,(ClickPosition.Y-470.f)/(1054.f-470.f)); //462.746 //1035.151
            UE_LOG(LogTemp,Warning,TEXT("ClickPosition.Y %f"),ClickPosition.Y);
            float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
            float WorldPositionY = FMath::Lerp(-23280.0,18780.0,NormalY); //1054.779541 //506.431274
            FVector WorldPosition;
            WorldPosition.X = WorldPositionX;
            WorldPosition.Y = WorldPositionY + 4000.f;
            WorldPosition.Z = 150.f; // 높이값 고정 또는 특정 레벨의 높이로 설정
            //카메라 높이에서 카메라 회전값을 넣은 LineTrace 
            DrawDebugLine(GetWorld(),WorldPosition,WorldPosition+FVector(0,0,10000),FColor::Red,true,1000);
            PlayerPawn->OnMouseRightMinimap(WorldPosition);
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
        UE_LOG(LogTemp,Warning,TEXT("ClickPosition.Y %f"),ClickPosition.Y);
        float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
        float WorldPositionY = FMath::Lerp(-23280.0,18780.0,NormalY); //1054.779541 //506.431274
        FVector WorldPosition;
        WorldPosition.X = WorldPositionX;
        WorldPosition.Y = WorldPositionY + 2000.f;
       UE_LOG(LogTemp,Error,TEXT("%f %f %s"), NormalX,NormalY,*ClickPosition.ToString());
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
    Super::NativePaint(Args,AllottedGeometry,MyCullingRect,OutDrawElements,LayerId,InWidgetStyle,bParentEnabled);
 //   FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	//CustomContext = &Context;
    return LayerId;
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
    UE_LOG(LogTemp,Warning,TEXT(" %f   %f"),NormalizedX,NormalizedY);
    float NormalX = FMath::Lerp(300.085f,0.f,NormalizedX);
    float NormalY = FMath::Lerp(950.f,420.f,NormalizedY); //462.746 //1035.151 1009.235107
     UE_LOG(LogTemp,Warning,TEXT(" %f   NormalY%f"),NormalX,NormalY);
    
    //UE_LOG(LogTemp,Warning,TEXT("Location.X %f OriginLocation.X %f  NormalizedY  %f"),(Location.X - OriginLocation.X), MiniMapWorldSize.X);
    //UE_LOG(LogTemp,Warning,TEXT("Location.X - OriginLocation.X %f.NormalizedY  %f"),(Location.Y - OriginLocation.Y),MiniMapWorldSize.Y);
    return FVector2D(NormalX,NormalY);
}

void UMinimapWidget::FindPath(const FVector& StartLocation,const FVector& TargetLocation, TArray<FVector>& ArrayLocation)
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if(nullptr == NavSystem)
        return;
    UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation);
    if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
        ArrayLocation = NavPath->PathPoints;
    }
}