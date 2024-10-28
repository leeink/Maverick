// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTask_SetState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LDG/SoldierAIController.h"

UBTTask_SetState::UBTTask_SetState()
{
	NodeName = TEXT("SetState");
}

EBTNodeResult::Type UBTTask_SetState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(ASoldierAIController* Controller = Cast<ASoldierAIController>(OwnerComp.GetAIOwner()))
	{
		Controller -> SetState(NewState);
		Controller -> GetBlackboardComponent() -> SetValueAsEnum(TEXT("State"), static_cast<uint8>(NewState));
		
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
