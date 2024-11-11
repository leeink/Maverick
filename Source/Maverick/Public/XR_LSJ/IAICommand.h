// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAICommand.generated.h"

DECLARE_DELEGATE(FDel_UnitDie);

UENUM(BlueprintType)
enum class EAIUnitCommandState : uint8
{
	IDLE  ,
	MOVE   ,
	ATTACK  ,
	DAMAGE  ,
	DIE  ,
	Defense,
};
UENUM(BlueprintType)
enum class EAIUnitCategory : uint8
{
	SQUAD UMETA(DisplayName = "분대"),
	TANK  UMETA(DisplayName = "탱크") ,
};
// This class does not need to be modified.
UINTERFACE(MinimalAPI,Blueprintable)
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
	virtual EAIUnitCommandState GetCurrentCommandState()=0;
	virtual void SetCommandState(EAIUnitCommandState Command){}
	virtual EAIUnitCategory GetAIUnitCategory() const { return AIUnitCategory; }
	virtual void SetAIUnitCategory(EAIUnitCategory val) { AIUnitCategory = val; }
	virtual FVector GetTargetLocation() { return FVector::ZeroVector; }
	//죽었을때 알려주는 델리게이트
	FDel_UnitDie FDelUnitDie;
};
