﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AISquadFSMComponent.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE UMETA(DisplayName = "대기") ,
	MOVE  UMETA(DisplayName = "이동") ,
	ATTACK  UMETA(DisplayName = "공격") ,
	DAMAGE UMETA(DisplayName = "데미지") ,
	DIE UMETA(DisplayName = "죽음") ,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAVERICK_API UAISquadFSMComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVector> PathVectorArray;
	int32 CurrentPathPointIndex;
	FVector SquadPosition;
	EEnemyState CurrentState = EEnemyState::IDLE;
public:	
	// Sets default values for this component's properties
	UAISquadFSMComponent();
	
	void SetState(EEnemyState NextState);
	UFUNCTION()
	void OnMoveCompleted(EPathFollowingResult::Type Result);
	void MovePathAsync(TArray<FVector>& NavPathArray);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void TickIdle(const float& DeltaTime);
	void TickMove(const float& DeltaTime);
	void TickAttack(const float& DeltaTime);
	void TickDamage(const float& DeltaTime);
	void TickDie(const float& DeltaTime);

	void MoveToTarget();
	UPROPERTY()
	class AAISquad* AISquadBody;
	//이동 위치
	UPROPERTY()
	FVector ArrivalPoint=FVector::ZeroVector;
	//공격 타겟
	UPROPERTY()
	class ACharacter* Target;
	//공격 사정거리
	float AttackDistance = 100.0f;
	// 네비게이션을 이용해서 길찾기를 하고싶다.
	UPROPERTY()
	class AAISquadController* AISquadController;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
public:
	FVector GetArrivalPoint() const { return ArrivalPoint; }
	void SetArrivalPoint(FVector val) { ArrivalPoint = val; }
	FVector GetSquadPosition() const { return SquadPosition; }
	void SetSquadPosition(FVector val) { SquadPosition = val; }
	EEnemyState GetCurrentState() const { return CurrentState; }
	void SetCurrentState(EEnemyState val) { CurrentState = val; }
};
