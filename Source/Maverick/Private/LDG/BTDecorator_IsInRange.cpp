// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTDecorator_IsInRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTDecorator_IsInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if(OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetActor"))));
	Distance = OwnerComp.GetAIOwner() -> GetPawn() -> GetDistanceTo(Target);
}

bool UBTDecorator_IsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(Distance <= 1000.0f)
	{
		return true;
	}
	
	return false;
}
