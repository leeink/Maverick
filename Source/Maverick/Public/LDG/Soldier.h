// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Soldier.generated.h"

class UWidgetComponent;
class UFlockingComponent;

DECLARE_DELEGATE(FDel_SoldierUnitDie);

UCLASS()
class MAVERICK_API ASoldier : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Decal, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* SelectedDecal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Flocking, meta = (AllowPrivateAccess = "true"))
	UFlockingComponent* FlockingComponent;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HealthWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SFX, meta = (AllowPrivateAccess = "true"))
	USoundBase* DieSound;

	UPROPERTY()
	class UArmyWidgetBase* ArmyWidgetInstance;

	UPROPERTY()
	class USoldierHealthWidget* SoldierHealthWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  PathKey, meta = (AllowPrivateAccess = "true"))
	int MinimapPathKey;

public:
	// Sets default values for this character's properties
	ASoldier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	//Death Count Delegate
	FDel_SoldierUnitDie Del_PlayerSoldierUnitDie;
	
	void Selected();
	void Deselected();
	void ToggleWidget(bool bShow);
	void ArmyWidgetBilboard();

	FORCEINLINE bool IsSelected() const { return bSelected; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE UArmyWidgetBase* GetArmyWidgetInstance() const { return ArmyWidgetInstance; }
	FORCEINLINE UFlockingComponent* GetFlockingComponent() const { return FlockingComponent; }
	FORCEINLINE int GetMinimapPathKey() const { return MinimapPathKey; }
	FORCEINLINE void SetMinimapPathKey(int NewKey) { MinimapPathKey = NewKey; }
};
