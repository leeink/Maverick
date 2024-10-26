// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIUnitHpBar.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UAIUnitHpBar : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(meta = (BindWidget))
	class UImage* MOS;
	UPROPERTY()
	class UTexture2D* SquadTexture;
	UPROPERTY()
	class UTexture2D* TankTexture;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBar;
public:
	UAIUnitHpBar(const FObjectInitializer& ObjectInitializer);
	void SetUISquadImage();
	void SetUITankImage();
	void SetHpBar(float Percent);

};
