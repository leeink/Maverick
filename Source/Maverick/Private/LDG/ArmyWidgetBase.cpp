// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/ArmyWidgetBase.h"

#include "Components/ProgressBar.h"

void UArmyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UArmyWidgetBase::UpdateHealthBar(float HealthPercentage)
{
	HealthBar->SetPercent(HealthPercentage);
}
