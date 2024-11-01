// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/UserControlIcon.h"
#include "Components/Image.h"


void UUserControlIcon::ChangeIconColor()
{
	
	FLinearColor IdleBaseColor = FLinearColor(0.348360,1.000000,0.225000,1.000000);
	FLinearColor IdleMaskColor = FLinearColor(0.050905,0.360000,0.000000,1.000000);
	FLinearColor ClickedBaseColor = FLinearColor(1.000000,0.267098,0.000000,1.000000);
	FLinearColor ClickedMaskColor = FLinearColor(0.360000,0.141776,0.000000,1.000000);

	img_Icon_Base->SetBrushTintColor(ClickedBaseColor);
	img_Icon_Mask2->SetBrushTintColor(ClickedBaseColor);
}
