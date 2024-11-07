// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoldierHealthWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class MAVERICK_API USoldierHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UFUNCTION()
	void UpdateHealth(float Health);
};
