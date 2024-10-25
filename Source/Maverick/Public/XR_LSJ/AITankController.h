// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AITankController.generated.h"
DECLARE_DELEGATE_OneParam(FTank_AIC_MoveCompleted, EPathFollowingResult::Type);
/**
 * 
 */
UCLASS()
class MAVERICK_API AAITankController : public AAIController
{
	GENERATED_BODY()

public:
	FTank_AIC_MoveCompleted FCallback_AIController_MoveCompleted;
	void OnMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);
};
