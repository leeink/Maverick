// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISquadBullet.generated.h"

UCLASS()
class MAVERICK_API AAISquadBullet : public AActor
{
	GENERATED_BODY()
	
	class UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* MovementComp;
	UPROPERTY(VisibleAnywhere,Category=Collision)
	class USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* BulletFXSystem;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* BulletFXComponent;
public:	
	// Sets default values for this actor's properties
	AAISquadBullet();
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void InitMovement(FVector Direction);
public:
	class UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
	void SetMovementComp(class UProjectileMovementComponent* val) { MovementComp = val; }
};
