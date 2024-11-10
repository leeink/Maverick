// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

UENUM(BlueprintType)
enum class EMOS : uint8
{
	Soldier,
	Tank,
	PlayerSoldier,
	PlayerTank,
};

UCLASS()
class MAVERICK_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	//병과
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess),Category = "MOS")
	EMOS MOS = EMOS::Soldier;
	//시작시 이동할 위치
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess),Category = "MOS")
	FVector StartLocation;
public:	
	// Sets default values for this actor's properties
	AEnemySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	EMOS GetMOS() const { return MOS; }
	void SetMOS(EMOS val) { MOS = val; }
	FVector GetStartLocation() const { return StartLocation; }
	void SetStartLocation(FVector val) { StartLocation = val; }
};
