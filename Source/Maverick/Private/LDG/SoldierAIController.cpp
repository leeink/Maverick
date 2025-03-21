// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/SoldierAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/FlockingComponent.h"
#include "LDG/RifleSoliderAnimInstance.h"
#include "LDG/Soldier.h"
#include "XR_LSJ/AITankPawn.h"

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
		//GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Idle Command"));
		SetState(EState::Idle);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(EState::Idle));
	}
}

void ASoldierAIController::MoveCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(EState::Move);

		//PossessedPawn -> GetFlockingComponent() -> SetDestination(GoalLocation);
		GetBlackboardComponent() -> SetValueAsVector(FName(TEXT("TargetLocation")), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(EState::Move));
	}
}

void ASoldierAIController::ChaseCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(EState::Chase);

		//PossessedPawn -> GetFlockingComponent() -> SetDestination(GoalLocation);
		GetBlackboardComponent() -> SetValueAsVector(FName(TEXT("TargetLocation")), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(EState::Chase));

		if(AActor* Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor")))))
		{
			SetState(EState::Attack);
		}
	}
}

void ASoldierAIController::AttackCommand()
{
	if(AActor* Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor")))))
	{
		UGameplayStatics::ApplyDamage(Target, Damage, GetInstigator() -> GetController(), GetOwner(), UDamageType::StaticClass());
		RifleAnimInstance -> PlayAttackMontage();
	}
}

void ASoldierAIController::Die()
{
	SetState(EState::Die);
	GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(EState::Die));
	PossessedPawn -> GetCapsuleComponent() -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	EObjectTypeQuery EnemyTraceType = UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel6);
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		PossessedPawn -> GetActorLocation(),
		PossessedPawn -> GetActorLocation(),
		DetectionRadius,
		{EnemyTraceType},
		false,
		{PossessedPawn},
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1f
		);

	if(HitResult.bBlockingHit && HitResult.GetActor() -> ActorHasTag(TEXT("Enemy")))
	{
		if(AAITankPawn* Unit = Cast<AAITankPawn>(HitResult.GetActor()))
		{
			if(Unit -> GetCurrentCommandState() == EAIUnitCommandState::DIE)
			{
				return;
			}
		}
		GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), HitResult.GetActor());
	}
	else
	{
		EnemyForget();
	}
}

void ASoldierAIController::EnemyForget()
{
	GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), nullptr);
}