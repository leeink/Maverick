// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/ResultValueSlotWidget.h"
#include "Components/TextBlock.h"

void UResultValueSlotWidget::InitSlot(FString Name, int32 KillEnemySoldierCount, int32 LostSoldier, int32 KillEnemyTankCount, int32 LostTank)
{
	Text_Name->SetText(FText::FromString(Name));

	Text_KillEnemySoldierCount->SetText(FText::FromString(FString::FromInt(KillEnemySoldierCount)));

	Text_LostSoldier->SetText(FText::FromString(FString::FromInt(LostSoldier)));

	Text_KillEnemyTankCount->SetText(FText::FromString(FString::FromInt(KillEnemyTankCount)));

	Text_LostTank->SetText(FText::FromString(FString::FromInt(LostTank)));
}
