// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTDecorator_IsDead.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "XR_LSJ/IAICommand.h"


bool UBTDecorator_IsDead::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetActor")))))
	{
		if(auto* commandTarget = Cast<IIAICommand>(Target))
		{
			if(commandTarget -> GetCurrentCommandState() != EAIUnitCommandState::DIE)
			{
				return true;
			}
		}
	}
	return false;
}
