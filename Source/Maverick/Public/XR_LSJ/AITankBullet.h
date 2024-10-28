// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AITankBullet.generated.h"

UCLASS()
class MAVERICK_API AAITankBullet : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* MovementComp;
	UPROPERTY(VisibleAnywhere,Category=Collision)
	class USphereComponent* CollisionComp;


	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* BulletEffectComp;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* BulletFXSystem;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* BulletFXComponent;
	float ExplosiveMaxDamage;
	float ExplosiveMinDamage;
	float ExplosiveRange;

public:	
	// Sets default values for this actor's properties
	AAITankBullet();
		UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* BulletEffect;
	void NotifyActorBeginOverlap(AActor* OtherActor);

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void DestroyBullet();
	UFUNCTION()
	void OnParticleSystemFinished(UParticleSystemComponent* FinishedComponent);
public:
	// Called every frame
	void InitMovement(FVector Direction);
	virtual void Tick(float DeltaTime) override;

	float GetExplosiveMinDamage() const { return ExplosiveMinDamage; }
	void SetExplosiveMinDamage(float val) { ExplosiveMinDamage = val; }
	float GetExplosiveRange() const { return ExplosiveRange; }
	void SetExplosiveRange(float val) { ExplosiveRange = val; }
	float GetExplosiveMaxDamage() const { return ExplosiveMaxDamage; }
	void SetExplosiveMaxDamage(float val) { ExplosiveMaxDamage = val; }
};
