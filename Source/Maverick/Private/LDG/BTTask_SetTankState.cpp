// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTask_SetTankState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LDG/TankAIController.h"

EBTNodeResult::Type UBTTask_SetTankState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(ATankAIController* SController = Cast<ATankAIController>(OwnerComp.GetAIOwner()))
	{
		SController -> SetState(NewState);
		SController -> GetBlackboardComponent() -> SetValueAsEnum(TEXT("State"), static_cast<uint8>(NewState));
		
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
