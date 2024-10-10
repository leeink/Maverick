// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadController.h"
#include "Navigation/PathFollowingComponent.h"

void AAISquadController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{ 
	Super::OnMoveCompleted(RequestID, Result);  
	FCallback_AIController_MoveCompleted.Broadcast(Result.Code); 
};