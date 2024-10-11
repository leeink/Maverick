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
protected:
	UPROPERTY(BlueprintReadWrite)
	EEnemyState CurrentState;
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeBeginPlay();
};
