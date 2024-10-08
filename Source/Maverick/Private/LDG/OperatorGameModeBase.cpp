// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/OperatorGameModeBase.h"

#include "LDG/OperatorPawn.h"
#include "LDG/OperatorPlayerController.h"
#include "LDG/OperatorSpectatorPawn.h"

AOperatorGameModeBase::AOperatorGameModeBase()
{
	DefaultPawnClass = AOperatorPawn::StaticClass();
	PlayerControllerClass = AOperatorPlayerController::StaticClass();
	SpectatorClass = AOperatorSpectatorPawn::StaticClass();
}
