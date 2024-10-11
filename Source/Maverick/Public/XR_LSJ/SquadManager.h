// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquadManager.generated.h"

UCLASS()
class MAVERICK_API ASquadManager : public AActor
{
	GENERATED_BODY()
	TArray<class AAISquad*> SquadArray;
	//�д�� ������ ���
	TArray<FVector> SquadPositionArray;
	//�ִ� �д�� ��
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 MaxSpawnCount = 6;
	//�̵� �׽�Ʈ
	FVector ArrivalPoint = FVector(800,0,0);
	//���� ��ġ ���
	TArray<FVector> ObstructionPoints;

public:
	//��ǥ���� �ִ� �Ÿ�
	UPROPERTY(EditDefaultsOnly)
	float MaxMoveLength = 10000.0f;
	// Sets default values for this actor's properties
	ASquadManager();
	//������ �д�� BP
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAISquad> SpawnSquadPactory;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//��ǥ�������� �浹�� Actor �� ��谪 ��ġ �������� 
	TArray<FVector> GetBoundingBoxPointsSortedByDistance(AActor* TargetActor, float Interval /*= 50.0f*/);
	//TargetLocation�� ���� ��θ� ã�� �д���鿡�� ����, ������ ������ �����Ѵ�.
	void FindPath(const FVector& TargetLocation);
	//���󹰷� ���� ��θ� ã�� �д���鿡�� ����, ������ ������ �����Ѵ�.
	void FindObstructionPath(TArray<FVector>& TargetLocation);
	//TargetLocation�� �̵��Ѵ�. ������ �ִٸ� ������ �� �ִ� ���� ����� ��ġ�� �̵��Ѵ�. 
	void CheckLocationForObject(const FVector& TargetLocation);
	//CheckLocationForObject �׽�Ʈ
	void CheckLocationForObject();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};
