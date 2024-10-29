// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlockingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAVERICK_API UFlockingComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float CohesionWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float SeparationWeight = 2.0f;  // 분리 가중치 증가

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float AlignmentWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float NeighborRadius = 300.0f;

	// 새로운 이동 관련 파라미터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float DesiredSeparation = 100.0f;  // 유닛 간 희망 거리

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float SteeringStrength = 2.0f;  // 조향력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 5.0f;  // 회전 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float LookAheadDistance = 100.0f;  // 앞을 보는 거리

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	bool bAlwaysLookAtTarget = true;  // 항상 목표물 바라보기 여부

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float MinDistanceToTarget = 50.0f;  // 목표 지점까지의 최소 거리

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	float MinSpeedForRotation = 10.0f;

	FVector CurrentDestination;
	bool bHasDestination;
	FVector CurrentVelocity;
	FVector LastMovementDirection;
	
public:	
	// Sets default values for this component's properties
	UFlockingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector CalculateCohesion(const TArray<AActor*>& Neighbors);
	FVector CalculateSeparation(const TArray<AActor*>& Neighbors);
	FVector CalculateAlignment(const TArray<AActor*>& Neighbors);
	FVector CalculateSeekForce(const FVector& TargetLocation);
	TArray<AActor*> GetNeighbors();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetDestination(FVector NewDestination);
	
	void UpdateRotation(float DeltaTime);
	FRotator CalculateTargetRotation() const;
	bool IsNearDestination() const;
	FVector GetLookAtLocation() const;
};
