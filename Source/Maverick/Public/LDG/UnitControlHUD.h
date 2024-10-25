// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UnitControlHUD.generated.h"

/**
 * 
 */

class AOperatorPawn;

UCLASS()
class MAVERICK_API AUnitControlHUD : public AHUD
{
	GENERATED_BODY()
	// OperatorPawn
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = OperatorPawn, meta = (AllowPrivateAccess = "true"))
	AOperatorPawn* OperatorPawn;
	
	// Mouse State
	bool bIsDrawing;
	FVector2D StartMousePosition;
	FVector2D CurrentMousePosition;

	// Draw Rectangle Units
	UPROPERTY()
	TArray<AActor*> InRectangleUnits;

	UPROPERTY()
	TArray<AActor*> InRectangleTanks;

	// Selected Units
	

public:
	AUnitControlHUD();
	
public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	void MarqueePressed();
	void MarqueeHeld();
	void MarqueeReleased();
};
