// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArmySelectWidget.generated.h"

class UUniformGridPanel;
class UImage;
/**
 * 
 */
UCLASS()
class MAVERICK_API UArmySelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* ActivePanel;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InActivePanel;
public:
	void ActivationWidget(int Index);
	void DeactivationWidget();
};
