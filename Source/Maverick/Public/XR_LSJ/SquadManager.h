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
	//분대원 포지션 목록
	TArray<FVector> SquadPositionArray;
	//최대 분대원 수
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 MaxSpawnCount = 6;
	//이동 테스트
	FVector ArrivalPoint = FVector(800,0,0);
	//엄폐 위치 목록
	TArray<FVector> ObstructionPoints;

public:
	//목표지점 최대 거리
	UPROPERTY(EditDefaultsOnly)
	float MaxMoveLength = 10000.0f;
	// Sets default values for this actor's properties
	ASquadManager();
	//스폰할 분대원 BP
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAISquad> SpawnSquadPactory;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//목표지점에서 충돌된 Actor 의 경계값 위치 가져오기 
	TArray<FVector> GetBoundingBoxPointsSortedByDistance(AActor* TargetActor, float Interval /*= 50.0f*/);
	//TargetLocation로 가는 경로를 찾아 분대원들에게 전달, 도착시 진형을 유지한다.
	void FindPath(const FVector& TargetLocation);
	//엄폐물로 가는 경로를 찾아 분대원들에게 전달, 도착시 진형을 유지한다.
	void FindObstructionPath(TArray<FVector>& TargetLocation);
	//TargetLocation로 이동한다. 엄폐물이 있다면 엄폐할 수 있는 가장 가까운 위치로 이동한다. 
	void CheckLocationForObject(const FVector& TargetLocation);
	//CheckLocationForObject 테스트
	void CheckLocationForObject();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};
