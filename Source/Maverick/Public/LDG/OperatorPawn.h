// Fill out your copyright notice in the Description page of Project Settings.

/*
 *	Author: Lee Dong Guen
 * 
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OperatorPawn.generated.h"

class AOperatorSpectatorPawn;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

struct FInputActionValue;

UCLASS()
class MAVERICK_API AOperatorPawn : public APawn
{
	GENERATED_BODY()

	// Components
	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;


	// Input
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* OperatorMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_MouseLeft;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SpawnSpectator;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchSlot1;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchSlot2;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchSlot3;

	// Spectator Pawn Queue
	TArray<AOperatorSpectatorPawn*> SpectatorPawnArray;

	//Scroll Speed
	UPROPERTY(EditDefaultsOnly, Category = ScrollSpeed, meta = (AllowPrivateAccess = "true"))
	float ScrollSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SpectatorClass, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AOperatorSpectatorPawn> SpectatorClass;
	
	// Mouse Position
	float MouseX;
	float MouseY;

	// Viewport Size
	int ViewPortX;
	int ViewPortY;

	// Pre Mouse Clicked Position
	FVector PreMousePosition;

public:
	// Sets default values for this pawn's properties
	AOperatorPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	void OnMouseLeft(const FInputActionValue& Value);
	void OnSpawnSpectator(const FInputActionValue& Value);
	void OnSwitchSlot1(const FInputActionValue& Value);
	void OnSwitchSlot2(const FInputActionValue& Value);
	void OnSwitchSlot3(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
