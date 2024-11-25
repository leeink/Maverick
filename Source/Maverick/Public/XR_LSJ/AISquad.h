// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IAICommand.h"
#include "AISquad.generated.h" 

DECLARE_DELEGATE(FDel_TargetDie);
DECLARE_DELEGATE_OneParam(FDel_InGameHidden,bool);
DECLARE_DELEGATE_OneParam(FDel_FailToDestination,int32);
DECLARE_DELEGATE_OneParam(FDel_SquadUnitDie,int32);
DECLARE_DELEGATE_OneParam(FDel_SquadUnitDamaged,float);
USTRUCT(Atomic,BlueprintType)
struct FSquadData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 Hp;
	UPROPERTY()
	float AttackSpeed;
	UPROPERTY()
	float MoveSpeed;
	UPROPERTY()
	float FindTargetRange;
};


UCLASS()
class MAVERICK_API AAISquad : public ACharacter, public IIAICommand
{
	GENERATED_BODY()
	UPROPERTY()
	FSquadData SquadAbility;

	int32 MySquadNumber;

	UPROPERTY(EditDefaultsOnly,Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* GunMeshComp;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Meta = (AllowPrivateAccess = true))
	class USoundBase* GunFireSound;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Meta = (AllowPrivateAccess = true))
	class USoundAttenuation* SA_Unit;
public:
	// Sets default values for this character's properties
	AAISquad();
	virtual FVector GetTargetLocation();
	//Bullet
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BulletFactory; 

	//Muzzle ����Ʈ
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* GunMuzzleFXSystem;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* GunMuzzleFXComponent;

	//������ ���� �� ó��
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//���ݽ� �Ѿ˰� ����Ʈ ��ȯ
	void AttackFire();
	UFUNCTION(BlueprintCallable)
	virtual EAIUnitCommandState GetCurrentCommandState() override;
	virtual void SetCommandState(EAIUnitCommandState Command) override;

	int32 GetMySquadNumber() const { return MySquadNumber; }
	void SetMySquadNumber(int32 val) { MySquadNumber = val; }
	class USkeletalMeshComponent* GetGunMeshComp() const { return GunMeshComp; }
	void SetGunMeshComp(class USkeletalMeshComponent* val) { GunMeshComp = val; }
	FSquadData GetSquadAbility() const { return SquadAbility; }
	void SetSquadAbility(FSquadData val) { SquadAbility = val; }

	void SetInGameHidden(bool val);
	UFUNCTION(BlueprintCallable)
	virtual void AddViewCount();
	UFUNCTION(BlueprintCallable)
	virtual void MinusViewCount();
	virtual int32 GetViewCount() const {return viewCount;}
	UFUNCTION(BlueprintCallable)
	void SetViewCountToHidden();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//��������Ʈ ����
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
	//�� Ÿ���� �׾����� SquadManager�� �˷��ִ� ��������Ʈ
	FDel_TargetDie FDelTargetDie;
	//�д���� �׾����� SquadManager�� �˷��ִ� ��������Ʈ
	FDel_SquadUnitDie FDelSquadUnitDie;
	//�д���� ������ �޾��� �� SquadManager�� �˷��ִ� ��������Ʈ
	FDel_SquadUnitDamaged FDelSquadUnitDamaged;
	//�������� �������� ������ �� SquadManager�� �˷��ִ� ��������Ʈ
	FDel_FailToDestination FDelFailToDestination;
	//�ΰ��ӿ��� �������ų� ������� �� SquadManager�� �˸��� ��������Ʈ
	FDel_InGameHidden FDelInGameHidden;
};
