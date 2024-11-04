// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/TankAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/Soldier.h"
#include "LDG/TankBase.h"

void ATankAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPawn = Cast<ATankBase>(InPawn);
	//RifleAnimInstance = Cast<URifleSoliderAnimInstance>(PossessedPawn -> GetMesh() -> GetAnimInstance());
	RunBehaviorTree(BehaviourTree);
	StartDetectionTimer();
}

void ATankAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PossessedPawn -> TurretRotation(UpdateTurret());
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

void ATankAIController::AttackCommand()
{
	if(AActor* Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor")))))
	{
		UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		500.f,
		Target -> GetActorLocation(),
		ExplosionRange,
		UDamageType::StaticClass(),
		{GetPawn()},
		GetPawn(),
		GetInstigator() -> GetController()
		);
		GEngine -> AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Attack"));
	}
}

float ATankAIController::UpdateTurret()
{
	AActor* Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor"))));
	if(Target != nullptr)
	{
		FVector CurrentLocation = PossessedPawn -> GetActorLocation();
		FVector DirectionToTarget = Target -> GetActorLocation() - PossessedPawn -> GetActorLocation().GetSafeNormal();

		float AngleBetween = FMath::Acos(FVector::DotProduct(PossessedPawn -> GetActorForwardVector(), DirectionToTarget));
		float AngleInDegrees = FMath::RadiansToDegrees(AngleBetween);
		float RotationSpeed = 90.f;
	
		return FMath::Min(RotationSpeed*GetWorld() -> GetDeltaSeconds(), AngleInDegrees);
	}
	return 0.f;
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
	EObjectTypeQuery EnemyTraceType = UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel6);
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		PossessedPawn -> GetActorLocation(),
		PossessedPawn -> GetActorLocation(),
		DetectionRadius,
		{EnemyTraceType},
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
	}
	else
	{
		EnemyForget();
	}
}

void ATankAIController::EnemyForget()
{
	GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), nullptr);
}
