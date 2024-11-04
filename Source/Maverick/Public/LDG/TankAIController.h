// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TankAIController.generated.h"

/**
 * 
 */

class ATankBase;

UENUM(BlueprintType)
enum class ETankState : uint8
{
	Idle = 0	UMETA(DisplayName = "Idle"),
	Move = 1	UMETA(DisplayName = "Move"),
	Chase = 2	UMETA(DisplayName = "Chase"),
	Attack = 3	UMETA(DisplayName = "Attack"),
	Die = 4		UMETA(DisplayName = "Die"),
};

UCLASS()
class MAVERICK_API ATankAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BehaviourTree, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviourTree;

	UPROPERTY()
	ATankBase* PossessedPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Detection, meta = (AllowPrivateAccess = "true"))
	float DetectionRadius = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Detection, meta = (AllowPrivateAccess = "true"))
	float ExplosionRange = 800.f;

	FTimerHandle DetectionTimerHandle;
	FTimerHandle ForgetTimerHandle;

public:
	ETankState CurrentState;

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	void SetState(ETankState NewState);

	void IdleCommand();
	void MoveCommand(FVector GoalLocation);
	void ChaseCommand(FVector GoalLocation);
	void AttackCommand();
	float UpdateTurret();
	
	void StartDetectionTimer();
	void EnemyDetection();
	void EnemyForget();

	/*FORCEINLINE URifleSoliderAnimInstance* GetRifleAnimInstance() const { return RifleAnimInstance; }*/
};
