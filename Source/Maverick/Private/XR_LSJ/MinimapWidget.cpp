// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/MinimapWidget.h"
#include "Kismet/GameplayStatics.h"

FReply UMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 클릭된 위치를 위젯 공간에서 좌표로 얻기
    FVector2D LocalClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

    // 클릭된 위치로 플레이어 이동
    MovePlayerToMapClick(LocalClickPosition);

    return FReply::Handled();
}
void UMinimapWidget::MovePlayerToMapClick(const FVector2D& ClickPosition)
{
    // 화면의 미니맵 중심을 기준으로 상대 위치 계산
    FVector2D MinimapCenter = FVector2D(0.5f * this->GetDesiredSize().X, 0.5f * this->GetDesiredSize().Y);
    FVector2D Offset = ClickPosition - MinimapCenter;

    // 플레이어 캐릭터를 가져와서 해당 위치로 이동 명령
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
    if (PlayerPawn)
    {
         // 오프셋을 월드 위치로 변환 
        float NormalX = FMath::Lerp(0.f,1.f,(ClickPosition.X-29.294f)/(374.085-29.294));
        float NormalY = FMath::Lerp(1.f,0.f,(ClickPosition.Y-462.746f)/(1035.151f-462.746f)); //462.746 //1035.151
         
        float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
        float WorldPositionY = FMath::Lerp(-21330.0,20440.0,NormalY);
        FVector WorldPosition;
        WorldPosition.X = WorldPositionX;
        WorldPosition.Y = WorldPositionY;
       UE_LOG(LogTemp,Error,TEXT("%f %f %s"), NormalX,NormalY,*ClickPosition.ToString());
        WorldPosition.Z = PlayerPawn->GetActorLocation().Z; // 높이값 고정 또는 특정 레벨의 높이로 설정
        PlayerPawn->SetActorLocation(WorldPosition);
    }



    
}