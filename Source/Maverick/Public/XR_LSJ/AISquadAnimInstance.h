// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AISquadFSMComponent.h"
#include "AISquadAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UAISquadAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	class AAISquad* AISquadBody;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* AttackAM;
protected:
	UPROPERTY(BlueprintReadWrite)
	bool IsAttacking = true;
	UPROPERTY(BlueprintReadWrite)
	EEnemyState CurrentState;
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeBeginPlay();
	UFUNCTION()
	virtual void AnimNotify_Fire();
public:

	class UAnimMontage* GetAttackAM() const { return AttackAM; }
	void SetAttackAM(class UAnimMontage* val) { AttackAM = val; }
	bool GetIsAttacking() const { return IsAttacking; }
	void SetIsAttacking(bool val) { IsAttacking = val; }
};
