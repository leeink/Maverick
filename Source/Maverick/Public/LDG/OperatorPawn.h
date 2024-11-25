// Fill out your copyright notice in the Description page of Project Settings.

/*
 *	Author: Lee Dong Guen
 * 
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OperatorPawn.generated.h"

class UArmySelectWidget;
class UBoxComponent;
class UNiagaraSystem;
class ATankBase;
class ARifleSoldier;
class ASoldier;
class AOperatorSpectatorPawn;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AUnitControlHUD;

struct FInputActionValue;

UCLASS()
class MAVERICK_API AOperatorPawn : public APawn
{
	GENERATED_BODY()

	// Components
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Collsion, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collsion;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	// Input
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* OperatorMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_MouseLeft;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_MouseRight;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_MouseWheelUp;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_MouseWheelDown;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SpawnSpectator;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchSlot1;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchSlot2;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchSlot3;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_AttackReady;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Ctrl;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ArmySlot1;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ArmySlot2;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ArmySlot3;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_StopAction;

	// LocationArray
	TArray<FVector> LocationArray;

	//Scroll Speed
	UPROPERTY(EditDefaultsOnly, Category = ScrollSpeed, meta = (AllowPrivateAccess = "true"))
	float ScrollSpeed;

	// Mouse Cursor Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* MoveCursorEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cursor, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* AttackCursorEffect;
	
	// Mouse Position
	float MouseX;
	float MouseY;

	// Viewport Size
	int ViewPortX;
	int ViewPortY;

	// Current Mouse Clicked Position
	FVector CurrentMousePosition;

	// LeftMouseClicking?
	bool bIsLeftMouseClick;

	// Attack Ready? (A - Clicked)
	bool bAttackReady;
	
	// Controlled Units
	UPROPERTY()
	AUnitControlHUD* UnitControlHUD;
	
	UPROPERTY()
	TArray<ASoldier*> SelectedUnits;

	UPROPERTY()
	TArray<ATankBase*> SelectedTanks;

	UPROPERTY()
	TArray<AActor*> ArmySlot1;

	UPROPERTY()
	TArray<AActor*> ArmySlot2;

	UPROPERTY()
	TArray<AActor*> ArmySlot3;

	// Army Control Variables
	bool bCtrlPressed;
	float LastClickTime = 0.f;
	float DoubleClickInterval = .2f;
	bool bWaitingForSecondClick;

	// Army Control Widgets
	UPROPERTY(EditDefaultsOnly, Category = ArmyControlWidget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ArmySelectWidgetClass;
	
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category = ArmyControlWidget, meta = (AllowPrivateAccess = "true"))
	UArmySelectWidget* ArmySelectWidget;
	
public:
	// Sets default values for this pawn's properties
	AOperatorPawn();
	void OnMouseRightMinimap(FVector Location);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	void OnMouseLeftStarted(const FInputActionValue& Value);
	void OnMouseLeftCompleted(const FInputActionValue& Value);
	void OnMouseRight(const FInputActionValue& Value);
	void OnMouseWheelUp(const FInputActionValue& Value);
	void OnMouseWheelDown(const FInputActionValue& Value);
	void OnSwitchSlot1(const FInputActionValue& Value);
	void OnSwitchSlot2(const FInputActionValue& Value);
	void OnSwitchSlot3(const FInputActionValue& Value);
	void OnAttackReady(const FInputActionValue& Value);
	void OnCtrlTriggered(const FInputActionValue& Value);
	void OnCtrlReleased(const FInputActionValue& Value);
	void OnSelectSlot1(const FInputActionValue& Value);
	void OnSelectSlot2(const FInputActionValue& Value);
	void OnSelectSlot3(const FInputActionValue& Value);
	void OnStopAction(const FInputActionValue& Value);
	void DoubleClickMoveLocation(FVector Location);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE TArray<ASoldier*>& GetSelectedUnits() { return SelectedUnits; }
	FORCEINLINE TArray<ATankBase*>& GetSelectedTanks() { return SelectedTanks; }
	FORCEINLINE TArray<AActor*>& GetArmySlot1() { return ArmySlot1; }
	FORCEINLINE TArray<AActor*>& GetArmySlot2() { return ArmySlot2; }
	FORCEINLINE TArray<AActor*>& GetArmySlot3() { return ArmySlot3; }
};
