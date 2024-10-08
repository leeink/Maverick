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
	TArray<FVector> SquadPositionArray;
	int32 MaxSpawnCount;
	FVector ArrivalPoint = FVector(800,0,0);
	TArray<FVector> TestPoint;

public:
	//��ǥ���� �ִ� �Ÿ�
	UPROPERTY(EditDefaultsOnly)
	float MaxMoveLength = 10000.0f;
	// Sets default values for this actor's properties
	ASquadManager();
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAISquad> SpawnSquadPactory;

	//��ǥ�������� �浹�� Actor �� ��谪 ��ġ �������� 
	TArray<FVector> GetBoundingBoxPointsSortedByDistance(AActor* MyActor, AActor* TargetActor, float Interval /*= 50.0f*/);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool UpDirectionPoint(AActor* BaseActor, FVector Point);
	void TestMove();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};
