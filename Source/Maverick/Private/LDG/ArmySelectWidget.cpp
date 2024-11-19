// Fill out your copyright notice in the Description page of Project Settings.

#include "LDG/ArmySelectWidget.h"
#include "Components/UniformGridPanel.h"


void UArmySelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DeactivationWidget();
}

void UArmySelectWidget::ActivationWidget(int Index)
{
	// 1. Deactivate all widgets
	DeactivationWidget();

	// 2. Activate the widget at the given index
	ActivePanel->GetChildAt(Index)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	InActivePanel->GetChildAt(Index)->SetVisibility(ESlateVisibility::Hidden);
}

void UArmySelectWidget::DeactivationWidget()
{
	for(auto* Child : ActivePanel->GetAllChildren())
	{
		Child->SetVisibility(ESlateVisibility::Hidden);
	}

	for(auto* Child : InActivePanel->GetAllChildren())
	{
		Child->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
