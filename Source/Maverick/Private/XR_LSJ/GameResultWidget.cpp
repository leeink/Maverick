// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/GameResultWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "XR_LSJ/ResultValueSlotWidget.h"

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UGameResultWidget::AddBasicSlot()
{
	auto* BasicSlotSouthKorea = CreateWidget(GetWorld(), ScrollBox_BasicSlot);
	auto* BasicSlotNorthKorea = CreateWidget(GetWorld(), ScrollBox_BasicSlot);
	ScrollBox_SouthKorea->AddChild(BasicSlotSouthKorea);
	ScrollBox_NorthKorea->AddChild(BasicSlotNorthKorea);
}

void UGameResultWidget::SetClearTime(FString Time)
{
	FString TimeText = L"플레이타임  - "+Time;
	Text_Time->SetText(FText::FromString(TimeText));
}

void UGameResultWidget::AddSouthKoreaData(FString Name, int32 KillEnemySoldierCount, int32 LostSoldier, int32 KillEnemyTankCount, int32 LostTank)
{
	UResultValueSlotWidget* SlotSouthKorea =Cast<UResultValueSlotWidget>(CreateWidget(GetWorld(),ScrollBox_ValueSlot));
	SlotSouthKorea->InitSlot(Name,KillEnemySoldierCount,LostSoldier,KillEnemyTankCount,LostTank);
	ScrollBox_SouthKorea->AddChild(SlotSouthKorea);
}

void UGameResultWidget::AddNorthKoreaData(FString Name, int32 KillEnemySoldierCount, int32 LostSoldier, int32 KillEnemyTankCount, int32 LostTank)
{
	UResultValueSlotWidget* SlotNorthKorea =Cast<UResultValueSlotWidget>(CreateWidget(GetWorld(),ScrollBox_ValueSlot));
	SlotNorthKorea->InitSlot(Name,KillEnemySoldierCount,LostSoldier,KillEnemyTankCount,LostTank);
	ScrollBox_NorthKorea->AddChild(SlotNorthKorea);
}
