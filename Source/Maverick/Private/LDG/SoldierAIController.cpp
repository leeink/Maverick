// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/SoldierAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/RifleSoliderAnimInstance.h"
#include "LDG/Soldier.h"

void ASoldierAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPawn = Cast<ASoldier>(InPawn);
	RifleAnimInstance = Cast<URifleSoliderAnimInstance>(PossessedPawn -> GetMesh() -> GetAnimInstance());
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
		
		GetBlackboardComponent() -> SetValueAsVector(FName(TEXT("TargetLocation")), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(EState::Move));
	}
}

void ASoldierAIController::ChaseCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(EState::Chase);

		GetBlackboardComponent() -> SetValueAsVector(FName(TEXT("TargetLocation")), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(EState::Chase));
	}
}

void ASoldierAIController::AttackCommand(AActor* TargetActor)
{
	AActor* Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor"))));
	UGameplayStatics::ApplyDamage(Target, 25.f, GetInstigator() -> GetController(), GetOwner(), UDamageType::StaticClass());
	RifleAnimInstance -> PlayAttackMontage();
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
		{PossessedPawn},
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1f
		);

	if(HitResult.bBlockingHit && HitResult.GetActor() -> ActorHasTag(TEXT("Enemy")))
	{
		GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), HitResult.GetActor());
		//FString TargetActorName = FString::Printf(TEXT("%s"), *HitResult.GetActor() -> GetName());
		//GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TargetActorName);
		GetWorldTimerManager().ClearTimer(ForgetTimerHandle);
	}
	else
	{
		GetWorldTimerManager().SetTimer(
		ForgetTimerHandle,
		this,
		&ASoldierAIController::EnemyForget,
		2.f,
		false
		);
	}
}

void ASoldierAIController::EnemyForget()
{
	GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), nullptr);
}