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
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBar;
public:
	void SetHpBar(float Percent);

};
