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

    FPaintContext* CustomContext;
    UPROPERTY(meta=(AllowPrivateAccess),BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> WarningUIClass;
public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    void CreateWarningUI(FVector Location);
protected:
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const;

    FVector2D ConvertingLocationToMinimap(FVector Location);
    void FindPath(const FVector& StartLocation, const FVector& TargetLocation, TArray<FVector>& ArrayLocation);
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Minimap")
    bool VisibleWarningUI;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldXScale = 25000.0f;  // �̴ϸ��� ���� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldYScale = 50000.0f;  // �̴ϸ��� ���� ������ ����
    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void MovePlayerToMapClick(const FVector2D& ClickPosition);
};
