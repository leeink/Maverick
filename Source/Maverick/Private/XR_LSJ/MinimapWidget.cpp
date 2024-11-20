// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/MinimapWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/OperatorPawn.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"

FReply UMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // Ŭ���� ��ġ�� ���� �������� ��ǥ�� ���
        FVector2D LocalClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

        // Ŭ���� ��ġ�� �÷��̾� �̵�
        MovePlayerToMapClick(LocalClickPosition);
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
       if (AOperatorPawn* PlayerPawn = Cast<AOperatorPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
       {
            FVector2D ClickPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

            // �������� ���� ��ġ�� ��ȯ 
            float NormalX = FMath::Lerp(0.f,1.f,(ClickPosition.X-29.294f)/(374.085-29.294));
            float NormalY = FMath::Lerp(1.f,0.f,(ClickPosition.Y-462.746f)/(1035.151f-462.746f)); //462.746 //1035.151
         
            float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
            float WorldPositionY = FMath::Lerp(-21330.0,20440.0,NormalY);
            FVector WorldPosition;
            WorldPosition.X = WorldPositionX;
            WorldPosition.Y = WorldPositionY;
            WorldPosition.Z = 150.f; // ���̰� ���� �Ǵ� Ư�� ������ ���̷� ����
            PlayerPawn->OnMouseRightMinimap(WorldPosition);
       }
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
        float NormalY = FMath::Lerp(1.f,0.f,(ClickPosition.Y-462.746f)/(1035.151f-462.746f)); //462.746 //1035.151
         
        float WorldPositionX = FMath::Lerp(11080.0,-11090.0,NormalX);
        float WorldPositionY = FMath::Lerp(-21330.0,20440.0,NormalY);
        FVector WorldPosition;
        WorldPosition.X = WorldPositionX;
        WorldPosition.Y = WorldPositionY;
       UE_LOG(LogTemp,Error,TEXT("%f %f %s"), NormalX,NormalY,*ClickPosition.ToString());
        WorldPosition.Z = PlayerPawn->GetActorLocation().Z; // ���̰� ���� �Ǵ� Ư�� ������ ���̷� ����
        PlayerPawn->SetActorLocation(WorldPosition);
    }
}