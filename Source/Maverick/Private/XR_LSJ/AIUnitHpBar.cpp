// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AIUnitHpBar.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UAIUnitHpBar::SetHpBar(float Percent)
{
	HpBar->SetPercent(Percent);
}
