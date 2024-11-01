// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/BTTas_TankShellShot.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/TankAIController.h"

UBTTas_TankShellShot::UBTTas_TankShellShot()
{
	NodeName = TEXT("Tank Shell Shot");
}

EBTNodeResult::Type UBTTas_TankShellShot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(ATankAIController* Controller = Cast<ATankAIController>(OwnerComp.GetAIOwner()))
	{
		Controller->AttackCommand();
		return EBTNodeResult::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast to ATankAIController"));
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}
