// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AISquad.generated.h" 

UCLASS()
class MAVERICK_API AAISquad : public ACharacter
{
	GENERATED_BODY()
	int32 MySquadNumber;
	UPROPERTY(EditDefaultsOnly,Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* GunMeshComp;
public:
	// Sets default values for this character's properties
	AAISquad();
	
	int32 GetMySquadNumber() const { return MySquadNumber; }
	void SetMySquadNumber(int32 val) { MySquadNumber = val; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FVector TestMovePoint = FVector(1000,0,0);
	void TestMove();
public:
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMesh* GunMesh;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UAISquadFSMComponent* FSMComp;

};
