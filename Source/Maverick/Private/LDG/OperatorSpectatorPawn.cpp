// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/OperatorSpectatorPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/OperatorPawn.h"
#include "LDG/OperatorPlayerController.h"

class UEnhancedInputLocalPlayerSubsystem;

void AOperatorSpectatorPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(NewController))
	{
		PlayerController -> SetInputMode(FInputModeGameOnly());
		PlayerController -> SetShowMouseCursor(false);

		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()) )
		{
			Subsystem->AddMappingContext(SpectatorMappingContext , 0);
		}
	}
}

void AOperatorSpectatorPawn::UnPossessed()
{
	Super::UnPossessed();
}

void AOperatorSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent -> BindAction(IA_SwitchToOperator, ETriggerEvent::Started, this, &AOperatorSpectatorPawn::SwitchToOperator);
	}
}

void AOperatorSpectatorPawn::SwitchToOperator(const FInputActionValue& Value)
{
	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController()))
	{
		UGameplayStatics::GetActorOfClass(GetWorld(), AOperatorPawn::StaticClass());
		PlayerController -> Possess(Cast<AOperatorPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AOperatorPawn::StaticClass())));
	}
}