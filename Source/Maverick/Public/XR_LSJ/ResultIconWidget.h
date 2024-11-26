// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UResultIconWidget : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Win;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Defeat;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Defeat;
public:
	void SetVisibleIcon(bool Win);
};
