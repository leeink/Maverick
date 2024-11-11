// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OccupiedLocation.generated.h"

UENUM(BlueprintType)
enum class EOccupiedLocationType : uint8
{
	Tank,
	Squad,
	Occupation
};

UCLASS()
class MAVERICK_API AOccupiedLocation : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess))
	bool IsTop;
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess))
	EOccupiedLocationType OccupiedLocationType;
public:	
	// Sets default values for this actor's properties
	AOccupiedLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	EOccupiedLocationType GetOccupiedLocationType() const { return OccupiedLocationType; }
	bool GetIsTop() const { return IsTop; }
};
