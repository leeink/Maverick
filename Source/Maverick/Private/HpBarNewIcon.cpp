// Fill out your copyright notice in the Description page of Project Settings.


#include "HpBarNewIcon.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
UHpBarNewIcon::UHpBarNewIcon(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> SquadTextureFinder(TEXT("/Game/Asset/ProIconPack/Textures/Generic_Icons/256x256/T_Friends_256x256.T_Friends_256x256"));
	if (SquadTextureFinder.Succeeded())
	{
		SquadTexture= SquadTextureFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> TankTextureFinder(TEXT("/Game/XR_LSJ/UI/Texture/tank.tank"));
	if (TankTextureFinder.Succeeded())
	{
		TankTexture=TankTextureFinder.Object;
	}
}

void UHpBarNewIcon::SetUISquadImage()
{
	//IconImage->SetBrushFromTexture(SquadTexture);
}

void UHpBarNewIcon::SetUITankImage()
{
	IconImage->SetBrushFromTexture(TankTexture);
	IconImage->SetRenderScale(FVector2D(.5,.5));
}

void UHpBarNewIcon::SetHpBar(float Percent)
{
	HpBarMainColor->SetPercent(Percent);
	HpBarSubColor->SetPercent(Percent);
	HpBarPattern->SetPercent(Percent);
}
