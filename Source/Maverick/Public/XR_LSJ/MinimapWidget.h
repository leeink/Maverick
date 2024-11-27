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
    UPROPERTY(meta=(BindWidget))
    class UImage* Minimap;
    TArray<TPair<FVector2D, FVector2D>> RuntimeLines;

    TMap<TArray<APawn*>,TArray<TPair<FVector2D, FVector2D>>> Unit_MinimapPath;
    TArray<TArray<APawn*>> Remove_MinimapPath;
    TMap<int32,TArray<TPair<FVector2D, FVector2D>>> Num_MinimapPath;
    TMap<int32,TArray<APawn*>> Num_Units;
    TArray<int32> RemoveMinimapPath;
public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    void CreateWarningUI(FVector Location);
protected:
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    void AddRuntimeLine(const FVector2D& Start, const FVector2D& End);
    FVector ConvertingMinimapToLocation(FVector2D Position);
    FVector2D ConvertingLocationToMinimap(FVector Location);
    void FindPath(const FVector& StartLocation, const FVector& TargetLocation, TArray<FVector>& ArrayLocation);
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Minimap")
    bool VisibleWarningUI;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldXScale = 25000.0f;  // 미니맵의 월드 스케일 조정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    float MinimapWorldYScale = 50000.0f;  // 미니맵의 월드 스케일 조정
    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void MovePlayerToMapClick(const FVector2D& ClickPosition);
};
