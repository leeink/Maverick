// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class MAVERICK_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	UPROPERTY()
	class UEnemyCount* EnemyCountWidget;
	int32 SoldierCount;
	int32 TankCount;
public:	
	// Sets default values for this actor's properties
	AEnemyManager();
	UFUNCTION()
	void DieSoldier();
	UFUNCTION()
	void DieTank();
	//ȭ�鿡 Enemy count ǥ��
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> EnemyCountClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ASquadManager> SquadManagerClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAITankPawn> TankPawnClass;
protected:

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
