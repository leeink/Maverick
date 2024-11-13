// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/TankAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LDG/TankBase.h"
#include "XR_LSJ/IAICommand.h"
#include "XR_LSJ/AITankPawn.h"

void ATankAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPawn = Cast<ATankBase>(InPawn);
	RunBehaviorTree(BehaviourTree);
	StartDetectionTimer();
}

void ATankAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
		
		if(PossessedPawn -> GetShotEffect() != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PossessedPawn -> GetShotEffect(), PossessedPawn -> GetMesh() -> GetSocketLocation(FName(TEXT("gun_1_jntSocket"))), FRotator::ZeroRotator, FVector(3.f));
		}
		if(PossessedPawn -> GetExplosionEffect() != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PossessedPawn -> GetExplosionEffect(), Target -> GetActorLocation(), FRotator::ZeroRotator, FVector(2.f));
		}
	}
}

float ATankAIController::GetRotationAngle()
{
	AActor * Target = Cast<AActor>(GetBlackboardComponent() -> GetValueAsObject(FName(TEXT("TargetActor"))));
	if(Target != nullptr)
	{
		FVector ForwardVector = PossessedPawn -> GetActorForwardVector();
		FVector TargetLocation = Target -> GetActorLocation();
		FVector TargetDir = UKismetMathLibrary::FindLookAtRotation(PossessedPawn -> GetActorLocation(), TargetLocation).Vector();

		FVector Cross = FVector::CrossProduct(ForwardVector, TargetDir.GetSafeNormal());
		float Dot = FVector::DotProduct(ForwardVector, TargetDir.GetSafeNormal());

		float AngleRadians = FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f));

		float sign = FMath::Sign(Cross.Z);
		return FMath::RadiansToDegrees(AngleRadians) * sign;
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

void ATankAIController::EnemyForget()
{
	GetBlackboardComponent() -> SetValueAsObject(FName(TEXT("TargetActor")), nullptr);
}
