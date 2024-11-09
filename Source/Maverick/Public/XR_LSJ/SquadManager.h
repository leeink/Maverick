// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XR_LSJ/AISquad.h"
#include "IAICommand.h"
#include "SquadManager.generated.h"

DECLARE_DELEGATE(FDel_SoldierUnitDie);

UENUM(BlueprintType)
enum class EObstructionDirection : uint8
{
	Left,
	Right,
	Down,
	Up
};

UCLASS()
class MAVERICK_API ASquadManager : public AActor, public IIAICommand
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class UBoxComponent* BoxComp;

	//���� Ÿ��
	TArray<AActor*> Target;
	//�д� �ɷ�ġ
	UPROPERTY()
	struct FSquadData SquadManagerAbility;
	//�д� ���� HP
	int32 CurrentSquadHp;
	//�д� �ִ� HP
	int32 MaxSquadHp; 
	//�д��� �д����
	TArray<class AAISquad*> SquadArray;
	//�д�� ������ ���
	TArray<FVector> SquadPositionArray;
	//�ִ� �д�� ��
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 MaxSpawnCount = 6;
	//���� ����ִ� �д�� ��
	int32 CurrentSquadCount = 6;
	//�̵� �׽�Ʈ
	FVector ArrivalPoint = FVector(1500,0,0);
	//���� ��ġ ���
	TArray<FVector> ObstructionPoints;
	//���� �����ϰ� �ִ� SquadNumber
	int32 CurrentAttachedSquadNumber;
	//ü�¹� UI Component
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess = true))
	class UWidgetComponent* HpWidgetComp;

	//���۽� �̵��� ��ǥ ����
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	FVector StartGoalLocation;

	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess = true))
	class UWidgetComponent* MinimapHpWidgetComp;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* MinimapHpWidgetSpringArm;
public:
	//�д�� ���� �˸� ��������Ʈ
	FDel_SoldierUnitDie FDelSoldierUnitDie;
	//ü�¹� UI Class
	UPROPERTY(EditDefaultsOnly,Category = "HpBar")
	TSubclassOf<class UHpBarNewIcon> MinimapHpWidgetClass;

	virtual FVector GetTargetLocation();
	//ü�¹� UI Class
	UPROPERTY(EditDefaultsOnly,Category = "HpBar")
	TSubclassOf<class UAIUnitHpBar> HpBarClass;
	//��ǥ���� �ִ� �Ÿ�
	UPROPERTY(EditDefaultsOnly)
	float MaxMoveLength = 10000.0f;
	// Sets default values for this actor's properties
	ASquadManager();
	//������ �д�� BP
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAISquad> SpawnSquadPactory;

	virtual EAIUnitCommandState GetCurrentCommandState() override;
	virtual void SetCommandState(EAIUnitCommandState Command) override;
	void SetMinimapUIZOrder(int32 Value);
	TArray<class AAISquad*> GetSquadArray() const { return SquadArray; }
	void SetSquadArray(TArray<class AAISquad*> val) { SquadArray = val; }
	int32 GetCurrentSquadCount() const { return CurrentSquadCount; }
	void SetCurrentSquadCount(int32 val) { CurrentSquadCount = val; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// PointA�� ��������� ������� ���ϴ� �Լ�
	bool IsCloserThan(const FVector& PointA, const FVector& PointB, const FVector& PointC);
	UFUNCTION()
	void FindCloseTargetPlayerUnit();
	// SquadManagerAbility.FindTargetRange ��ŭ ���� Ž���ϰ� ����� �� ������ ����
	UFUNCTION()
	void FindCloseTargetUnit();
	// SquadManagerAbility.FindTargetRange ��ŭ ���� Ž���ϰ� �� �д븦 ����
	void FindTargetSquad();
	// �� �д������ ���� Ÿ�� ���� ����
	void AttackTargetUnit();
	// �� �д������ �д� Ÿ�� ���� ����
	void AttackTargetSquad(AActor* TargetActor);
	//��ǥ�������� �浹�� Actor �� ��谪 ��ġ �������� 
	TArray<FVector> GetSurfacePointsOnRotatedBoundingBox(AActor* TargetActor, float Interval /*= 50.0f*/);
	TArray<FVector> GetSurfacePointsOnRotatedBoxComp(AActor* TargetActor, float Interval /*= 50.0f*/);
	//TargetLocation�� ���� ��θ� ã�� �д���鿡�� ����, ������ ������ �����Ѵ�.
	void FindPath(const FVector& TargetLocation);
	//���󹰷� ���� ��θ� ã�� �д���鿡�� ����, ������ ������ �����Ѵ�.
	void FindObstructionPath(TArray<FVector>& TargetLocation);
	//TargetLocation�� �̵��Ѵ�. ������ �ִٸ� ������ �� �ִ� ���� ����� ��ġ�� �̵��Ѵ�. 
	void CheckLocationForObject(const FVector& TargetLocation);
	//CheckLocationForObject �׽�Ʈ
	void CheckLocationForObject();
	//�д���� �������� ������ ü�¹ٿ� ����ȭ
	UFUNCTION()
	void DamagedSquadUnit(float Damage);
	//ü�¹� ǥ���ϴ� �д���� �׾��ٸ� ����ִ� �ٸ� �д������ ����
	UFUNCTION()
	void DieSquadUnit(int32 SquadNumber);
	//�д���� �̵� �Ұ����� ��ġ�� �޾Ҵٸ� �д��� ������ ��ȿ�� ��ġ�� �ش�.
	UFUNCTION()
	void MoveToValidDestination(int32 SquadNumber);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//������ ��ġ�� �д�� ������ ������ ��� ��ġ�� ����
	void MakeObstructionPoint(AActor* TargetActor, TArray<FVector>& OutPoints, EObstructionDirection DirectionNum);

	//������ ��ġ ����
	void GeneratePointsBetweenTwoCorners(const FVector& P1, const FVector& P2, float Interval, TArray<FVector>& OutPoints);
	
};
