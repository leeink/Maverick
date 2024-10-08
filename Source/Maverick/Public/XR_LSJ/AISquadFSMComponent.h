// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "AITypes.h" 
#include "Navigation/PathFollowingComponent.h" 
#include "AISquadFSMComponent.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE UMETA(DisplayName = "���") ,
	MOVE  UMETA(DisplayName = "�̵�") ,
	ATTACK  UMETA(DisplayName = "����") ,
	DAMAGE UMETA(DisplayName = "������") ,
	DIE UMETA(DisplayName = "����") ,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAVERICK_API UAISquadFSMComponent : public UActorComponent
{
	GENERATED_BODY()
	class UNavigationPath* CurrentPath;
	int32 CurrentPathPointIndex;
public:	
	// Sets default values for this component's properties
	UAISquadFSMComponent();
	EEnemyState State = EEnemyState::IDLE;
	void SetState(EEnemyState NextState);

	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	void MovePathAsync(UNavigationPath* NavPath);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void TickIdle(const float& DeltaTime);
	void TickMove(const float& DeltaTime);
	void TickAttack(const float& DeltaTime);
	void TickDamage(const float& DeltaTime);
	void TickDie(const float& DeltaTime);


	void MoveToArrivalPoint();
	void MoveToTarget();
	UPROPERTY()
	class AAISquad* AISquadBody;
	//�̵� ��ġ
	UPROPERTY()
	FVector ArrivalPoint=FVector::ZeroVector;
	//���� Ÿ��
	UPROPERTY()
	class ACharacter* Target;
	//���� �����Ÿ�
	float AttackDistance = 100.0f;
	// �׺���̼��� �̿��ؼ� ��ã�⸦ �ϰ�ʹ�.
	UPROPERTY()
	class AAIController* AISquadController;

	FVector PatrolPoint;
	float PatrolPointRadius = 500;
	// ����ġ���� �ݰ� 5����
	bool SetPatrolPoint(FVector origin, float radius, FVector& dest);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
public:
	FVector GetArrivalPoint() const { return ArrivalPoint; }
	void SetArrivalPoint(FVector val) { ArrivalPoint = val; }
};
