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
	class UGameResultWidget* GameResultWidget;
	class UUserControlUI* UserControlUI;
	int32 MaxSoldierCount;
	int32 MaxTankCount;
	int32 SoldierCount;
	int32 TankCount;

	int32 MaxPlayerSoldierCount;
	int32 MaxPlayerTankCount;
	int32 PlayerSoldierCount;
	int32 PlayerTankCount;
public:	
	// Sets default values for this actor's properties
	AEnemyManager();
	UFUNCTION()
	void DieSoldier();
	UFUNCTION()
	void DiePlayerSoldier();
	UFUNCTION()
	void DiePlayerTank();
	void ShowResult();
	UFUNCTION()
	void DieTank();
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> PlayerControllerClass;
	//화면에 Enemy count 표시
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> EnemyCountClass;
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> GameResultClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ASquadManager> SquadManagerClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAITankPawn> TankPawnClass;
		UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ATankBase> PlayerTankPawnClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ASoldier> PlayerSoldierPawnClass;
protected:

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
