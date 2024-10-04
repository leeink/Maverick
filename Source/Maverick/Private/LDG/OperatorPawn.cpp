// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/OperatorPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "LDG/OperatorPlayerController.h"

// Sets default values
AOperatorPawn::AOperatorPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera -> SetRelativeRotation(FRotator(-40.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void AOperatorPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AOperatorPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(NewController))
	{
		PlayerController -> SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways));
		PlayerController -> SetShowMouseCursor(true);
		
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()) )
		{
			Subsystem->AddMappingContext(OperatorMappingContext , 0);
		}

		
	}
}

void AOperatorPawn::UnPossessed()
{
	Super::UnPossessed();

	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController()))
	{
		PlayerController -> SetShowMouseCursor(false);

		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()) )
		{
			Subsystem->RemoveMappingContext(OperatorMappingContext);
		}
	}
}


// Called every frame
void AOperatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController()))
	{
		PlayerController -> GetMousePosition(MouseX, MouseY);
		PlayerController -> GetViewportSize(ViewPortX, ViewPortY);

		if(MouseX <= 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorWorldOffset(FVector(0.f, deltaOffset,0.f));
		}
		else if(MouseX >= static_cast<float>(ViewPortX) - 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorWorldOffset(FVector(0.f, -1 * deltaOffset,0.f));
		}

		if(MouseY <= 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorWorldOffset(FVector(-1 * deltaOffset, 0.f,0.f));
		}
		else if(MouseY >= static_cast<float>(ViewPortY) - 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorWorldOffset(FVector(deltaOffset, 0.f,0.f));
		}
	}
}

// Called to bind functionality to input
void AOperatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_SpawnSpectator , ETriggerEvent::Started , this , &AOperatorPawn::OnSpawnSpectator);
		EnhancedInputComponent->BindAction(IA_SwitchSlot1 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot1);
		EnhancedInputComponent->BindAction(IA_SwitchSlot2 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot2);
		EnhancedInputComponent->BindAction(IA_SwitchSlot3 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot3);
	}
}

void AOperatorPawn::OnSpawnSpectator(const FInputActionValue& Value)
{
}

void AOperatorPawn::OnSwitchSlot1(const FInputActionValue& Value)
{
}

void AOperatorPawn::OnSwitchSlot2(const FInputActionValue& Value)
{
}

void AOperatorPawn::OnSwitchSlot3(const FInputActionValue& Value)
{
}