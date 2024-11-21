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

    UPROPERTY(meta=(AllowPrivateAccess),BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> WarningUIClass;
public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    void CreateWarningUI(FVector Location);
protected:
    FVector2D ConvertingLocationToMinimap(FVector Location);
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Minimap")
    bool VisibleWarningUI;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldXScale = 25000.0f;  // 미니맵의 월드 스케일 조정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldYScale = 50000.0f;  // 미니맵의 월드 스케일 조정
    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void MovePlayerToMapClick(const FVector2D& ClickPosition);
};
