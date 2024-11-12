// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/GameResultWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "XR_LSJ/ResultValueSlotWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "XR_LSJ/ResultIconWidget.h"

void UGameResultWidget::ToMainLevel()
{
	FString Option = TEXT("?game=/Game/XR_LSJ/Maps/LSJ_BP_OperatorGameModeBase.LSJ_BP_OperatorGameModeBase_C");
	FString LevelPath = L"/Game/XR_LSJ/Maps/MainMenu.MainMenu";
	UGameplayStatics::OpenLevel(GetWorld(),*LevelPath,true,Option);

	//RemoveFromParent();
}

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Close->OnClicked.AddDynamic(this, &UGameResultWidget::ToMainLevel);

}
void UGameResultWidget::SetResultIcon(bool Value)
{
	UResultIconWidget* Icon = Cast<UResultIconWidget>(WBP_Result_Icon);
	if(Icon)
		Icon->SetVisibleIcon(Value);
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
