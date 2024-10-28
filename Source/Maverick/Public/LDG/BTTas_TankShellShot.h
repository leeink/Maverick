// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTas_TankShellShot.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UBTTas_TankShellShot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTas_TankShellShot();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
