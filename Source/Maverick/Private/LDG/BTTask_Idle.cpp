// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTask_Idle.h"

UBTTask_Idle::UBTTask_Idle()
{
	NodeName = TEXT("SetIdle");
}

EBTNodeResult::Type UBTTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
