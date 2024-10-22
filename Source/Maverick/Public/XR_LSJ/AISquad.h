// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AISquad.generated.h" 

USTRUCT(Atomic,BlueprintType)
struct FSquadData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float MaxHp;
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

	//Muzzle ¿Ã∆Â∆Æ
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* GunMuzzleFXSystem;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* GunMuzzleFXComponent;

	void SpawnBullet();
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
	FVector TestMovePoint = FVector(1000,0,0);
	void TestMove();

public:
	float GetLookTargetAngle(FVector TargetLocation);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UAISquadFSMComponent* FSMComp;

};
