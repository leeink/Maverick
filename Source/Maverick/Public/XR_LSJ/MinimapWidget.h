// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldXScale = 25000.0f;  // �̴ϸ��� ���� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldYScale = 50000.0f;  // �̴ϸ��� ���� ������ ����
    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void MovePlayerToMapClick(const FVector2D& ClickPosition);
};
