// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AITankController.h"
#include "Navigation/PathFollowingComponent.h"

void AAITankController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{ 
	Super::OnMoveCompleted(RequestID, Result);
	if (FCallback_AIController_MoveCompleted.IsBound())
	{
		FCallback_AIController_MoveCompleted.Execute(Result.Code); 
	}
};