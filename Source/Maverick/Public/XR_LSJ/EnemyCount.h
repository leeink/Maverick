// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyCount.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UEnemyCount : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UTextBlock* SoldierCount;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UTextBlock* TankCount;
public:
	//화면에 표시할 SoldierCount 설정
	UFUNCTION(BlueprintCallable)
	void SetSoldierCount(int Amount);
	//화면에 표시할 TankCount 설정
	UFUNCTION(BlueprintCallable)
	void SetTankCount(int Amount);
};
