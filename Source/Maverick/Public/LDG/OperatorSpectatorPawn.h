// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "OperatorSpectatorPawn.generated.h"


class UInputMappingContext;
class UInputAction;

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class MAVERICK_API AOperatorSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SpectatorMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SwitchToOperator;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	void SwitchToOperator(const FInputActionValue& Value);

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
