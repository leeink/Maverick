// Fill out your copyright notice in the Description page of Project Settings.


#include "UserControlUI.h"
#include "XR_LSJ/UserControlIcon.h"
#include "Components/TextBlock.h"

void UUserControlUI::NativeConstruct()
{
	ActivateColor = FLinearColor(1.000000,0.,0.,1.000000);
	DeactivateColor = FLinearColor(1.000000,1.,1.,1.000000);
	ClickedColor = FLinearColor(1.000000,0.052,0.,1.000000);
}

void UUserControlUI::SetMoveActivateColor()
{
	WB_Move->SetColorAndOpacity(ActivateColor);
}

void UUserControlUI::SetAttackActivateColor()
{
	WB_Attack->SetColorAndOpacity(ActivateColor);
}

void UUserControlUI::SetAttackDeactivateColor()
{
	WB_Attack->SetColorAndOpacity(DeactivateColor);
}

void UUserControlUI::SetAttackClickedColor()
{
	WB_Attack->SetColorAndOpacity(ClickedColor);
}

void UUserControlUI::SetMoveDeactivateColor()
{
	WB_Move->SetColorAndOpacity(DeactivateColor);
}

void UUserControlUI::SetMoveClickedColor()
{
	WB_Move->SetColorAndOpacity(ClickedColor);
}

void UUserControlUI::SetStopActivateColor()
{
	WB_Stop->SetColorAndOpacity(ActivateColor);
}

void UUserControlUI::SetStopDeactivateColor()
{
	WB_Stop->SetColorAndOpacity(DeactivateColor);
}

void UUserControlUI::SetSoldierCount(int Amount)
{
	SoldierCount->SetText(FText::AsNumber(Amount));
}

void UUserControlUI::SetTankCount(int Amount)
{
	TankCount->SetText(FText::AsNumber(Amount));
}
