// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "TankBase.generated.h"

class UWidgetComponent;
class UFloatingPawnMovement;
class UNavigationInvokerComponent;
class UBoxComponent;
/**
 * 
 */
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

public:
	ATankBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	void Selected();
	void Deselected();
	void ArmyWidgetBilboard();

	FORCEINLINE bool IsSelected() const { return bSelected; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
};
