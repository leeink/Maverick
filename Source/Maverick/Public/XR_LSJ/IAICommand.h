// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAICommand.generated.h"
UENUM(BlueprintType)
enum class EAIUnitCommandState : uint8
{
	IDLE UMETA(DisplayName = "���") ,
	MOVE  UMETA(DisplayName = "�̵�") ,
	ATTACK  UMETA(DisplayName = "����") ,
	DAMAGE UMETA(DisplayName = "������") ,
	DIE UMETA(DisplayName = "����") ,
};
UENUM(BlueprintType)
enum class EAIUnitCategory : uint8
{
	SQUAD UMETA(DisplayName = "�д�"),
	TANK  UMETA(DisplayName = "��ũ") ,
};
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIAICommand : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MAVERICK_API IIAICommand
{
	GENERATED_BODY()
protected:
	EAIUnitCommandState CurrentCommandState;
	EAIUnitCategory AIUnitCategory;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual EAIUnitCommandState GetCurrentCommandState(){ return EAIUnitCommandState();}
	virtual void SetCommandState(EAIUnitCommandState Command){}
	EAIUnitCategory GetAIUnitCategory() const { return AIUnitCategory; }
	void SetAIUnitCategory(EAIUnitCategory val) { AIUnitCategory = val; }
};
