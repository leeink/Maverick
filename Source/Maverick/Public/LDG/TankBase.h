// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.generated.h"

class UWidgetComponent;
class UFloatingPawnMovement;
class UNavigationInvokerComponent;
class UBoxComponent;
/**
 * 
 */

DECLARE_DELEGATE(FDel_TankUnitDie);

UCLASS()
class MAVERICK_API ATankBase : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Decal, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* SelectedDecal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Invoker, meta = (AllowPrivateAccess = "true"))
	UNavigationInvokerComponent* NavigationInvoker;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FloatingMovement, meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* FloatingMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	bool bSelected;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AttackPower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AttackRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ArmyWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* MiniMapWidget;

	UPROPERTY()
	class UArmyWidgetBase* ArmyWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = VFX, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ShotEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = VFX, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = VFX, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* DestroyEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SFX, meta = (AllowPrivateAccess = "true"))
	USoundBase* DestroySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DamagedMesh, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> DamagedMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  PathKey, meta = (AllowPrivateAccess = "true"))
	int MinimapPathKey;
	
public:
	ATankBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//Death Count Delegate
	FDel_TankUnitDie Del_PlayerTankUnitDie;
	
	void Selected();
	void Deselected();
	void ArmyWidgetBilboard();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRotate(float Degree);

	FORCEINLINE bool IsSelected() const { return bSelected; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE UParticleSystem* GetShotEffect() const { return ShotEffect; }
	FORCEINLINE UParticleSystem* GetExplosionEffect() const { return ExplosionEffect; }
	FORCEINLINE int GetMinimapPathKey() const { return MinimapPathKey; }
	FORCEINLINE void SetMinimapPathKey(int NewKey) { MinimapPathKey = NewKey; }
};
