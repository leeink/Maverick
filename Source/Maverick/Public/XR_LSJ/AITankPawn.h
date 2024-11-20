// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IAICommand.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITankPawn.generated.h"

DECLARE_DELEGATE(FDel_TankUnitDie);

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
	float ExplosiveMaxDamage;
	UPROPERTY()
	float ExplosiveMinDamage;
	UPROPERTY()
	float MoveSpeed;
	UPROPERTY()
	float FindTargetRange;
	UPROPERTY()
	float ExplosiveRange;
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
	EAIUnitCommandState PreState;
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
	int32 CurrentTankHp;
	int32 MaxTankHp;
	FTimerHandle FindEnemy;

	//시작시 이동할 목표 지점
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	FVector StartGoalLocation;

	EAIUnitCommandState CurrentTankState;
public:
	//탱크 죽음 알림 델리게이트
	FDel_TankUnitDie FDelTankUnitDie;
	// Sets default values for this pawn's properties
	AAITankPawn();
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void RotateYawurret(float Angle);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void RotatePitchTurret(float Angle);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void MoveWheelAnimation(float Speed);

	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void DieAnimation(bool Active);
	UFUNCTION(BlueprintImplementableEvent, Category="Custom")
    void FireFx(bool Active);
	UFUNCTION(BlueprintCallable)
	virtual EAIUnitCommandState GetCurrentCommandState() override;
	UFUNCTION(BlueprintCallable)
	virtual EAIUnitCommandState GetCurrentTankState();
	void SetMinimapUIZOrder(int32 Value);
	virtual void SetCommandState(EAIUnitCommandState Command) override;
	virtual void SetTankState(EAIUnitCommandState Command);
	//Bullet
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BulletFactory; 

	UFUNCTION(BlueprintCallable)
	virtual void FindPath(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable)
	virtual void AddViewCount();
	UFUNCTION(BlueprintCallable)
	virtual void MinusViewCount();

	virtual int32 GetViewCount() const {return viewCount;}
	UFUNCTION(BlueprintCallable)
	void SetViewCountToHidden();
protected:
	void SetVisibleTank(bool val);
	void FireCannon();
	float GetLookTargetAngle(FVector TargetLocation);
	UFUNCTION()
	void StopAttack();
	void AttackTargetUnit(AActor* TargetActor);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason);
	
	UFUNCTION()
	virtual void OnMoveCompleted(EPathFollowingResult::Type Result);
	virtual void MovePathAsync(TArray<FVector>& NavPathArray);
	bool CalculateBallisticVelocity(const FVector& StartLocation, 
    const FVector& EndLocation, 
    float DesiredTime, // 목표 도달 시간
    FVector& OutVelocity);
	virtual void FindCloseTargetUnit();

	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class UWidgetComponent* HpWidgetComp;
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess = true))
	class UWidgetComponent* MinimapHpWidgetComp;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* MinimapHpWidgetSpringArm;
public:
	virtual FVector GetTargetLocation();
	void FindCloseTargetPlayerUnit();
	//체력바 UI Class
	UPROPERTY(EditDefaultsOnly,Category = "HpBar")
	TSubclassOf<class UAIUnitHpBar> HpBarClass;
	//체력바 UI Class
	UPROPERTY(EditDefaultsOnly,Category = "HpBar")
	TSubclassOf<class UHpBarNewIcon> MinimapHpWidgetClass;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
};
