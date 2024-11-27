// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTask_Die.h"

#include "LDG/RifleSoliderAnimInstance.h"
#include "LDG/SoldierAIController.h"

EBTNodeResult::Type UBTTask_Die::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(ASoldierAIController* con = Cast<ASoldierAIController>(OwnerComp.GetAIOwner()))
	{
		con -> GetRifleAnimInstance() -> Montage_Stop(.25f);
		con -> GetRifleAnimInstance() -> PlayDeathMontage();

		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}
