// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TimerManager.h"
#include "SoldierAIController.generated.h"

class URifleSoliderAnimInstance;
class ASoldier;
class UBehaviorTree;
/**
 * 
 */
UENUM(BlueprintType)
enum class EState : uint8
{
	Idle = 0	UMETA(DisplayName = "Idle"),
	Move = 1	UMETA(DisplayName = "Move"),
	Chase = 2	UMETA(DisplayName = "Chase"),
	Attack = 3	UMETA(DisplayName = "Attack"),
	Die = 4		UMETA(DisplayName = "Die"),
};

UCLASS()
class MAVERICK_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BehaviourTree, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviourTree;

	UPROPERTY()
	ASoldier* PossessedPawn;

	UPROPERTY()
	URifleSoliderAnimInstance* RifleAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float Health;

	FTimerHandle DetectionTimerHandle;
	FTimerHandle ForgetTimerHandle;

public:
	EState CurrentState;

public:
	virtual void OnPossess(APawn* InPawn) override;

	void SetState(EState NewState);

	void IdleCommand();
	void MoveCommand(FVector GoalLocation);
	void ChaseCommand(FVector GoalLocation);
	void AttackCommand(AActor* TargetActor);
	void Die();
	void StartDetectionTimer();
	void EnemyDetection();
	void EnemyForget();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
