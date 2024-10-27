// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AIUnitHpBar.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

UAIUnitHpBar::UAIUnitHpBar(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> SquadTextureFinder(TEXT("/Game/TA_NYH/Icons/Military_Category/Soldier_non_selected_Enemy.Soldier_non_selected_Enemy"));
	if (SquadTextureFinder.Succeeded())
	{
		SquadTexture= SquadTextureFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> TankTextureFinder(TEXT("/Game/TA_NYH/Icons/Military_Category/Tank_non_selected_Enemy.Tank_non_selected_Enemy"));
	if (TankTextureFinder.Succeeded())
	{
		TankTexture=TankTextureFinder.Object;
	}
}
void UAIUnitHpBar::SetUISquadImage()
{
	MOS->SetBrushFromTexture(SquadTexture);
}
void UAIUnitHpBar::SetUITankImage()
{
	MOS->SetBrushFromTexture(TankTexture);
}
void UAIUnitHpBar::SetHpBar(float Percent)
{
	HpBar->SetPercent(Percent);
}
