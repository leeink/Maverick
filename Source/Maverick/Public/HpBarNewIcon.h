// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HpBarNewIcon.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UHpBarNewIcon : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	UPROPERTY()
	class UTexture2D* SquadTexture;
	UPROPERTY()
	class UTexture2D* TankTexture;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBarMainColor;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBarSubColor;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBarPattern;
public:
	UHpBarNewIcon(const FObjectInitializer& ObjectInitializer);
	void SetUISquadImage();
	void SetUITankImage();
	void SetHpBar(float Percent);

};
