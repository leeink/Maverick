// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/ResultIconWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
void UResultIconWidget::SetVisibleIcon(bool Win)
{
	if(Win)
		Text_Win->SetVisibility(ESlateVisibility::Visible);
	else
	{
		PlayAnimation(Defeat);
		Text_Defeat->SetVisibility(ESlateVisibility::Visible);
	}
		
}
