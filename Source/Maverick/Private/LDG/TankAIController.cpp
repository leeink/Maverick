// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/TankAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/TankBase.h"

void ATankAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPawn = Cast<ATankBase>(InPawn);
	//RifleAnimInstance = Cast<URifleSoliderAnimInstance>(PossessedPawn -> GetMesh() -> GetAnimInstance());
	RunBehaviorTree(BehaviourTree);
	StartDetectionTimer();
}

void ATankAIController::SetState(ETankState NewState)
{
	CurrentState = NewState;
}

void ATankAIController::IdleCommand()
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(ETankState::Idle);
	}
}

void ATankAIController::MoveCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(ETankState::Move);
		
		GetBlackboardComponent() -> SetValueAsVector(FName(TEXT("TargetLocation")), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(ETankState::Move));
	}
}

void ATankAIController::ChaseCommand(FVector GoalLocation)
{
	if(PossessedPawn -> IsSelected())
	{
		SetState(ETankState::Chase);

		GetBlackboardComponent() -> SetValueAsVector(FName(TEXT("TargetLocation")), GoalLocation);
		GetBlackboardComponent() -> SetValueAsEnum(FName(TEXT("State")), static_cast<uint8>(ETankState::Chase));
	}
}

void ATankAIController::AttackCommand(AActor* TargetActor)
{
	AActor* Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor"))));
	UGameplayStatics::ApplyDamage(Target, 25.f, GetInstigator() -> GetController(), GetOwner(), UDamageType::StaticClass());
	//RifleAnimInstance -> PlayAttackMontage();
}

void ATankAIController::StartDetectionTimer()
{
	GetWorld() -> GetTimerManager().SetTimer(DetectionTimerHandle,
		this,
		&ATankAIController::EnemyDetection,
		0.4f,
		true);
}

void ATankAIController::EnemyDetection()
{
	FHitResult HitResult;
	
	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		PossessedPawn -> GetActorLocation(),
		PossessedPawn -> GetActorLocation(),
		5000.f,
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
		&ATankAIController::EnemyForget,
		2.f,
		false
		);
	}
}

void ATankAIController::EnemyForget()
{
	GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), nullptr);
}
