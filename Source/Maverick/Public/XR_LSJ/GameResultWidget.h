// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox_SouthKorea;
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox_NorthKorea;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Time;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;

	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> ScrollBox_BasicSlot;
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> ScrollBox_ValueSlot;
protected:
	virtual void NativeConstruct() override;
public:
	void AddBasicSlot();
	void SetClearTime(FString Time);
	void AddSouthKoreaData(FString Name,int32 KillEnemySoldierCount, int32 LostSoldier,int32 KillEnemyTankCount, int32 LostTank);
	void AddNorthKoreaData(FString Name,int32 KillEnemySoldierCount, int32 LostSoldier,int32 KillEnemyTankCount, int32 LostTank);
};
