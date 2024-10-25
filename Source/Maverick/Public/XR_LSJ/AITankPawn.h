// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IAICommand.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITankPawn.generated.h"
USTRUCT(Atomic,BlueprintType)
struct FTankData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float Hp;
	UPROPERTY()
	float AttackSpeed;
	UPROPERTY()
	float MoveSpeed;
	UPROPERTY()
	float FindTargetRange;
};
UCLASS()
class MAVERICK_API AAITankPawn : public APawn , public IIAICommand
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess))
	class USkeletalMeshComponent* MeshComp;
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess))
	class UBoxComponent* BoxComp;
	class AAITankController* AITankController;
	struct FTankData TankAbility;
	int32 CurrentHp;
	int32 MaxHp;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UFloatingPawnMovement* MovementComponent;
	TArray<FVector> PathVectorArray;
	int32 CurrentPathPointIndex;
	AActor* Target;
	float TurretRotation;
	float TurretRotSpeed;
	float TurretRotTotalTime;
	float TurretRotAlpha;
	float FireCoolTime;
	float FireTotalTime;
public:
	// Sets default values for this pawn's properties
	AAITankPawn();
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void RotateYawurret(float Angle);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void RotatePitchTurret(float Angle);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void MoveWheelAnimation(float Speed);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void MoveWheelFX(float Intensity);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void DieAnimation(bool Active);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void FireFx(bool Active);
	virtual EAIUnitCommandState GetCurrentCommandState() override;
	virtual void SetCommandState(EAIUnitCommandState Command) override;
	//Bullet
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BulletFactory; 
protected:
	void FireCannon();
	float GetLookTargetAngle(FVector TargetLocation);
	void AttackTargetUnit(AActor* TargetActor);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnMoveCompleted(EPathFollowingResult::Type Result);
	virtual void MovePathAsync(TArray<FVector>& NavPathArray);
	virtual void FindCloseTargetUnit();
	virtual void FindPath(const FVector& TargetLocation);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
