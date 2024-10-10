// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Soldier.generated.h"

UCLASS()
class MAVERICK_API ASoldier : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AttackPower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AttackRate;

public:
	// Sets default values for this character's properties
	ASoldier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Wait();
	virtual void Move(FVector GoalLocation);
	virtual void Attack();
};
