// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTas_TankShellShot.h"

UBTTas_TankShellShot::UBTTas_TankShellShot()
{
}

EBTNodeResult::Type UBTTas_TankShellShot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
