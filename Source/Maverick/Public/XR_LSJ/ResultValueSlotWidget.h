// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultValueSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UResultValueSlotWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Name;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_KillEnemySoldierCount;
		UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_LostSoldier;
		UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_KillEnemyTankCount;
		UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_LostTank;

public:
	void InitSlot(FString Name, int32 KillEnemySoldierCount, int32 LostSoldier, int32 KillEnemyTankCount, int32 LostTank);
};
