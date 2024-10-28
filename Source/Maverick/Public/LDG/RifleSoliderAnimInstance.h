// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RifleSoliderAnimInstance.generated.h"

/**
 * 
 */

class UAnimMontage;

UCLASS()
class MAVERICK_API URifleSoliderAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;
	
public:
	virtual void NativeInitializeAnimation() override;

public:
	void PlayAttackMontage();
	void PlayDeathMontage();
};
