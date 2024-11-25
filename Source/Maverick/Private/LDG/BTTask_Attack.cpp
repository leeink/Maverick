// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTask_Attack.h"

#include "LDG/SoldierAIController.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(ASoldierAIController* Controller =  Cast<ASoldierAIController>(OwnerComp.GetAIOwner()))
	{
		Controller->AttackCommand();
		return EBTNodeResult::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast to ASoldierAIController"));
		return EBTNodeResult::Failed;
	}
}
