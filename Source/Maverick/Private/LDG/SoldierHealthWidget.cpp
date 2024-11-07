// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/SoldierHealthWidget.h"

#include "Components/ProgressBar.h"

void USoldierHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USoldierHealthWidget::UpdateHealth(float HealthPercentage)
{
	HealthBar -> SetPercent(HealthPercentage);
}
