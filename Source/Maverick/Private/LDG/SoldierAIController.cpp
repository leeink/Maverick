// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/SoldierAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/Soldier.h"

void ASoldierAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPawn = Cast<ASoldier>(InPawn);

	RunBehaviorTree(BehaviourTree);
	StartDetectionTimer();
}

void ASoldierAIController::SetState(EState NewState)
{
	CurrentState = NewState;
}

void ASoldierAIController::IdleCommand()
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(EState::Idle);
	}
}

void ASoldierAIController::MoveCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(EState::Move);

		GetBlackboardComponent() -> SetValueAsVector(TEXT("TargetLocation"), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(TEXT("State"), static_cast<uint8>(EState::Move));
	}
}

void ASoldierAIController::ChaseCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(EState::Chase);

		GetBlackboardComponent() -> SetValueAsVector(TEXT("TargetLocation"), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(TEXT("State"), static_cast<uint8>(EState::Chase));
	}
}

void ASoldierAIController::AttackCommand(AActor* TargetActor)
{
	UGameplayStatics::ApplyDamage(TargetActor, 25.f, this, GetOwner(), UDamageType::StaticClass());
}

void ASoldierAIController::StartDetectionTimer()
{
	GetWorld() -> GetTimerManager().SetTimer(DetectionTimerHandle,
		this,
		&ASoldierAIController::EnemyDetection,
		0.4f,
		true);
}

void ASoldierAIController::EnemyDetection()
{
	FHitResult HitResult;
	
	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		PossessedPawn -> GetActorLocation(),
		PossessedPawn -> GetActorLocation(),
		1000.f,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1f
		);

	if(HitResult.bBlockingHit && HitResult.GetActor() -> ActorHasTag(TEXT("Enemy")))
	{
		if(HitResult.GetActor() -> ActorHasTag(TEXT("Enemy")))
		{
			GetBlackboardComponent() -> SetValueAsObject(TEXT("TargetActor"), HitResult.GetActor());
		}
		else
		{
			GetBlackboardComponent() -> SetValueAsObject(TEXT("TargetActor"), nullptr);
		}
	}
}
