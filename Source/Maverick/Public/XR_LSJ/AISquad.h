// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AISquad.generated.h" 

DECLARE_DELEGATE(FDel_TargetDie);
DECLARE_DELEGATE(FDel_SquadUnitDie);

USTRUCT(Atomic,BlueprintType)
struct FSquadData
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
class MAVERICK_API AAISquad : public ACharacter
{
	GENERATED_BODY()
	UPROPERTY()
	FSquadData SquadAbility;

	int32 MySquadNumber;

	UPROPERTY(EditDefaultsOnly,Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* GunMeshComp;

public:
	// Sets default values for this character's properties
	AAISquad();

	//Bullet
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BulletFactory; 

	//Muzzle 이펙트
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* GunMuzzleFXSystem;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* GunMuzzleFXComponent;

	//데미지 받을 때 처리
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//공격시 총알과 이펙트 소환
	void AttackFire();

	int32 GetMySquadNumber() const { return MySquadNumber; }
	void SetMySquadNumber(int32 val) { MySquadNumber = val; }
	class USkeletalMeshComponent* GetGunMeshComp() const { return GunMeshComp; }
	void SetGunMeshComp(class USkeletalMeshComponent* val) { GunMeshComp = val; }
	FSquadData GetSquadAbility() const { return SquadAbility; }
	void SetSquadAbility(FSquadData val) { SquadAbility = val; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//델리게이트 해제
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	FVector TestMovePoint = FVector(1000,0,0);
	void TestMove();

public:
	float GetLookTargetAngle(FVector TargetLocation);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UAISquadFSMComponent* FSMComp;
	//적 타겟이 죽었을때 SquadManager에 알려주는 델리게이트
	FDel_TargetDie FDelTargetDie;
	//분대원이 죽었을때 SquadManager에 알려주는 델리게이트
	FDel_SquadUnitDie FDelSquadUnitDie;
};
