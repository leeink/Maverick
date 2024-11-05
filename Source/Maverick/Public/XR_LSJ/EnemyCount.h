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
	//ȭ�鿡 ǥ���� SoldierCount ����
	UFUNCTION(BlueprintCallable)
	void SetSoldierCount(int Amount);
	//ȭ�鿡 ǥ���� TankCount ����
	UFUNCTION(BlueprintCallable)
	void SetTankCount(int Amount);
};
