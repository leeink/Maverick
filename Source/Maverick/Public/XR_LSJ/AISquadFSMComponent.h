// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AISquadFSMComponent.generated.h"

#define BaseAttackRotatorYaw 17;

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
	//공격 타겟
	AActor* Target;
	//공격 중인가?
	bool IsAttacking = false;
	//공격 쿨타임
	float AttackCoolTime = 2.0f;
	float AttackCurrentTime = 2.0f;
	
	//목표 지점을 도착하기 위한 구간 경로 위치 배열
	UPROPERTY()
	TArray<FVector> PathVectorArray;
	//현재 경로 인덱스
	int32 CurrentPathPointIndex;
	//진형 위치
	FVector SquadPosition;
	//현재 상태
	EEnemyState CurrentState = EEnemyState::IDLE;
	//애니메이션 인스턴스
	class UAISquadAnimInstance* AISquadAnimInstance;

public:	
	// Sets default values for this component's properties
	UAISquadFSMComponent();
	//상태 지정
	void SetState(EEnemyState NextState);
	//지정한 위치에 도달했을때 목표 지점이면 Idle 상태로 멈춤 / 목표지점으로 이동 
	UFUNCTION()
	void OnMoveCompleted(EPathFollowingResult::Type Result);
	//NavPathArray의 위치에 따라 경로 이동
	void MovePathAsync(TArray<FVector>& NavPathArray);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//상대를 향해 바라본다.
	void TurnCanLookTarget(const float& DeltaTime);
	//상체만 상대를 향해 바라본다.
	void RotateUpperbodyToTarget(const float& DeltaTime);
	
	void TickIdle(const float& DeltaTime);
	void TickMove(const float& DeltaTime);
	void TickAttack(const float& DeltaTime);
	void TickDamage(const float& DeltaTime);
	void TickDie(const float& DeltaTime);
	//공격 시작
	void StartAttack();
	//공격 끝
	void EndAttack();
	//상대를 향해 이동
	void MoveToTarget();
	//분대원 캐릭터
	UPROPERTY()
	class AAISquad* AISquadBody;
	//이동 위치
	UPROPERTY()
	FVector ArrivalPoint=FVector::ZeroVector;

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
	AActor* GetTarget() const { return Target; }
	void SetTarget(AActor* val) { Target = val; }
	float GetAttackCurrentTime() const { return AttackCurrentTime; }
	void SetAttackCurrentTime(float val) { AttackCurrentTime = val; }
	bool GetIsAttacking() const { return IsAttacking; }
	void SetIsAttacking(bool val, AActor* TargetActor);
};
