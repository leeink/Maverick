// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadController.h"
#include "Navigation/PathFollowingComponent.h"

void AAISquadController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{ 
	Super::OnMoveCompleted(RequestID, Result);
	if (FCallback_AIController_MoveCompleted.IsBound())
	{
		FCallback_AIController_MoveCompleted.Broadcast(Result.Code); 
	}
	//UE_LOG(LogTemp, Warning, TEXT("2 MovePathAsync %s"),*K2_GetPawn()->GetName());
};