// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/FlockingComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFlockingComponent::UFlockingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bHasDestination = false;
	CurrentVelocity = FVector::ZeroVector;
}


// Called when the game starts
void UFlockingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UFlockingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	AActor* Owner = GetOwner();
    if (!Owner) return;

    TArray<AActor*> Neighbors = GetNeighbors();
    FVector SteeringForce = FVector::ZeroVector;
    
    // 기존 플로킹 및 이동 로직
    FVector SeparationForce = CalculateSeparation(Neighbors) * SeparationWeight;
    SteeringForce += SeparationForce;

    if (Neighbors.Num() > 0)
    {
        FVector CohesionForce = CalculateCohesion(Neighbors) * CohesionWeight;
        FVector AlignmentForce = CalculateAlignment(Neighbors) * AlignmentWeight;
        SteeringForce += CohesionForce + AlignmentForce;
    }

    if (bHasDestination && !IsNearDestination())
    {
        FVector SeekForce = CalculateSeekForce(CurrentDestination);
        SteeringForce += SeekForce;
    }

    SteeringForce = SteeringForce.GetClampedToMaxSize(SteeringStrength);
    CurrentVelocity += SteeringForce * DeltaTime;
    CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(MaxSpeed);

    // 이동 처리
    if (!CurrentVelocity.IsNearlyZero())
    {
        FVector NewLocation = Owner->GetActorLocation() + CurrentVelocity * DeltaTime;
        Owner->SetActorLocation(NewLocation);

        // 충분한 속도로 움직일 때만 방향 업데이트
        if (CurrentVelocity.Size() >= MinSpeedForRotation)
        {
            // 이동 방향 저장 (Z축 제외)
            FVector NewDirection = CurrentVelocity;
            NewDirection.Z = 0.0f;
            
            if (!NewDirection.IsNearlyZero())
            {
                LastMovementDirection = NewDirection.GetSafeNormal();
                
                // 부드러운 회전
                FRotator TargetRotation = LastMovementDirection.Rotation();
                FRotator NewRotation = FMath::RInterpTo(
                    Owner->GetActorRotation(),
                    FRotator(0.0f, TargetRotation.Yaw, 0.0f),
                    DeltaTime,
                    RotationSpeed
                );
                
                Owner->SetActorRotation(NewRotation);
            }
        }
    }
    // 정지 상태에서는 마지막 이동 방향 유지
    else if (!LastMovementDirection.IsNearlyZero())
    {
        FRotator CurrentRotation = Owner->GetActorRotation();
        Owner->SetActorRotation(FRotator(0.0f, CurrentRotation.Yaw, 0.0f));
    }
}

FVector UFlockingComponent::CalculateCohesion(const TArray<AActor*>& Neighbors)
{
	if (Neighbors.Num() == 0) return FVector::ZeroVector;
    
	FVector CenterPoint = FVector::ZeroVector;
	for (AActor* Neighbor : Neighbors)
	{
		CenterPoint += Neighbor->GetActorLocation();
	}
    
	CenterPoint /= Neighbors.Num();
	return (CenterPoint - GetOwner()->GetActorLocation()).GetSafeNormal();
}

FVector UFlockingComponent::CalculateSeparation(const TArray<AActor*>& Neighbors)
{
	FVector SeparationForce = FVector::ZeroVector;
	AActor* Owner = GetOwner();
    
	for (AActor* Neighbor : Neighbors)
	{
		FVector Direction = Owner->GetActorLocation() - Neighbor->GetActorLocation();
		float Distance = Direction.Size();
        
		if (Distance < DesiredSeparation && Distance > 0)
		{
			// 거리가 가까울수록 더 강한 분리력 적용
			float Strength = FMath::Clamp(1.0f - (Distance / DesiredSeparation), 0.0f, 1.0f);
			SeparationForce += Direction.GetSafeNormal() * (Strength * SteeringStrength);
		}
	}
    
	return SeparationForce;
}

FVector UFlockingComponent::CalculateAlignment(const TArray<AActor*>& Neighbors)
{
	if (Neighbors.Num() == 0) return FVector::ZeroVector;
    
	FVector AverageVelocity = FVector::ZeroVector;
	for (AActor* Neighbor : Neighbors)
	{
		AverageVelocity += Neighbor->GetVelocity();
	}
    
	AverageVelocity /= Neighbors.Num();
	return AverageVelocity.GetSafeNormal();
}

FVector UFlockingComponent::CalculateSeekForce(const FVector& TargetLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner) return FVector::ZeroVector;

	FVector DesiredVelocity = (TargetLocation - Owner->GetActorLocation()).GetSafeNormal() * MaxSpeed;
	return (DesiredVelocity - CurrentVelocity);
}

TArray<AActor*> UFlockingComponent::GetNeighbors()
{
	TArray<AActor*> Neighbors;
	TArray<AActor*> AllUnits;
    
	// 같은 타입의 모든 유닛 찾기
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), GetOwner()->GetClass(), AllUnits);
    
	for (AActor* Unit : AllUnits)
	{
		if (Unit != GetOwner())
		{
			float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Unit->GetActorLocation());
			if (Distance <= NeighborRadius)
			{
				Neighbors.Add(Unit);
			}
		}
	}
    
	return Neighbors;
}

void UFlockingComponent::SetDestination(FVector NewDestination)
{
	CurrentDestination = NewDestination;
	bHasDestination = true;
}

void UFlockingComponent::UpdateRotation(float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	FRotator CurrentRotation = Owner->GetActorRotation();
	FRotator TargetRotation = CalculateTargetRotation();
    
	// 부드러운 회전 보간
	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		RotationSpeed
	);
    
	Owner->SetActorRotation(NewRotation);
}

FRotator UFlockingComponent::CalculateTargetRotation() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return FRotator::ZeroRotator;

	FVector LookAtLocation = GetLookAtLocation();
	FVector Direction = (LookAtLocation - Owner->GetActorLocation()).GetSafeNormal();
    
	return Direction.Rotation();
}

bool UFlockingComponent::IsNearDestination() const
{
	AActor* Owner = GetOwner();
	if (!Owner || !bHasDestination) return false;

	float DistanceToTarget = FVector::Distance(Owner->GetActorLocation(), CurrentDestination);
	return DistanceToTarget <= MinDistanceToTarget;
}

FVector UFlockingComponent::GetLookAtLocation() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return FVector::ZeroVector;

	if (bAlwaysLookAtTarget && bHasDestination)
	{
		// 항상 목표 지점을 바라봄
		return CurrentDestination;
	}
	else
	{
		// 이동 방향을 바라봄
		if (!CurrentVelocity.IsNearlyZero())
		{
			return Owner->GetActorLocation() + CurrentVelocity.GetSafeNormal() * LookAheadDistance;
		}
		// 정지 상태에서는 현재 방향 유지
		return Owner->GetActorLocation() + Owner->GetActorForwardVector() * LookAheadDistance;
	}
}

