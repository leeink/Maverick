// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/EnemyCount.h"
#include "Components/TextBlock.h"
void UEnemyCount::SetSoldierCount(int Amount)
{
	SoldierCount->SetText(FText::AsNumber(Amount));
}

void UEnemyCount::SetTankCount(int Amount)
{
	TankCount->SetText(FText::AsNumber(Amount));
}