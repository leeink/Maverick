// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserControlIcon.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UUserControlIcon : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UImage* img_Icon_Base;
		UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UImage* img_Icon_Mask2;
	public:
	void ChangeIconColor();


};
