// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AISquadController.generated.h"

/**
 * 
 */

 DECLARE_MULTICAST_DELEGATE_OneParam(FSQUAD_AIC_MoveCompleted, EPathFollowingResult::Type);

UCLASS()
class MAVERICK_API AAISquadController : public AAIController
{
	GENERATED_BODY()
protected:



public:

	FSQUAD_AIC_MoveCompleted FCallback_AIController_MoveCompleted;
	// Overriding base 
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
