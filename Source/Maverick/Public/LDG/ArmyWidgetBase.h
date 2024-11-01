// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArmyWidgetBase.generated.h"

/**
 * 
 */

class UProgressBar;

UCLASS()
class MAVERICK_API UArmyWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UFUNCTION()
	void UpdateHealthBar(float HealthPercentage);
};
