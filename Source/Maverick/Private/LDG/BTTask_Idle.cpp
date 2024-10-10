// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTask_Idle.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_Idle::UBTTask_Idle()
{
	NodeName = TEXT("SetIdle");
}

EBTNodeResult::Type UBTTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(ACharacter* ControlledPawn = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}
