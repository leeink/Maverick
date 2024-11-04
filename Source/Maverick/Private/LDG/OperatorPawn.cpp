// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/OperatorPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/OperatorPlayerController.h"
#include "LDG/OperatorSpectatorPawn.h"
#include "LDG/RifleSoldier.h"
#include "LDG/SoldierAIController.h"
#include "LDG/TankAIController.h"
#include "LDG/TankBase.h"
#include "LDG/UnitControlHUD.h"

class ATankAIController;
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
	Camera -> SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void AOperatorPawn::BeginPlay()
{
	Super::BeginPlay();

	
	UnitControlHUD = Cast<AUnitControlHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0) -> GetHUD());
}

void AOperatorPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(NewController))
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController -> SetShowMouseCursor(true);
		PlayerController -> SetInputMode(InputMode);
		
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
		if(bIsLeftMouseClick)
		{
			UnitControlHUD -> MarqueeHeld();
		}
		
		PlayerController -> GetMousePosition(MouseX, MouseY);
		PlayerController -> GetViewportSize(ViewPortX, ViewPortY);

		if(MouseX <= 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorLocalOffset(FVector(0.f, -1 * deltaOffset,0.f));
		}
		else if(MouseX >= static_cast<float>(ViewPortX) - 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorLocalOffset(FVector(0.f, deltaOffset,0.f));
		}

		if(MouseY <= 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorLocalOffset(FVector(deltaOffset, 0.f,0.f));
		}
		else if(MouseY >= static_cast<float>(ViewPortY) - 10.f)
		{
			float deltaOffset = ScrollSpeed * GetWorld() -> GetDeltaSeconds();
			AddActorLocalOffset(FVector(-1 * deltaOffset, 0.f,0.f));
		}
	}
}

// Called to bind functionality to input
void AOperatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_MouseLeft , ETriggerEvent::Started , this , &AOperatorPawn::OnMouseLeftStarted);
		EnhancedInputComponent->BindAction(IA_MouseLeft , ETriggerEvent::Completed , this , &AOperatorPawn::OnMouseLeftCompleted);
		EnhancedInputComponent->BindAction(IA_MouseRight , ETriggerEvent::Started , this , &AOperatorPawn::OnMouseRight);
		EnhancedInputComponent->BindAction(IA_MouseWheelUp , ETriggerEvent::Triggered , this , &AOperatorPawn::OnMouseWheelUp);
		EnhancedInputComponent->BindAction(IA_MouseWheelDown , ETriggerEvent::Triggered , this , &AOperatorPawn::OnMouseWheelDown);
		EnhancedInputComponent->BindAction(IA_SpawnSpectator , ETriggerEvent::Started , this , &AOperatorPawn::OnSpawnSpectator);
		EnhancedInputComponent->BindAction(IA_SwitchSlot1 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot1);
		EnhancedInputComponent->BindAction(IA_SwitchSlot2 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot2);
		EnhancedInputComponent->BindAction(IA_SwitchSlot3 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot3);
		EnhancedInputComponent->BindAction(IA_AttackReady , ETriggerEvent::Started , this , &AOperatorPawn::OnAttackReady);
	}
}

void AOperatorPawn::OnMouseLeftStarted(const FInputActionValue& Value)
{
	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController()))
	{
		if(bAttackReady)
		{
			FHitResult HitResult;
			PlayerController -> GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
			if(HitResult.bBlockingHit)
			{
				CurrentMousePosition = HitResult.Location;
			}

			for(auto* Unit: SelectedUnits)
			{
				if(ASoldierAIController* SoldierController = Cast<ASoldierAIController>(Unit -> GetController()))
				{
					SoldierController -> ChaseCommand(HitResult.Location);
				}
			}

			for(auto* Tank: SelectedTanks)
			{
				if(ATankAIController* TankAIController = Cast<ATankAIController>(Tank -> GetController()))
				{
					//GEngine -> AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Chase Command"));
					TankAIController -> ChaseCommand(HitResult.Location);
				}
			}
			if(AttackCursorEffect != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackCursorEffect, HitResult.Location, FRotator::ZeroRotator);
			}
			
			bAttackReady = false;
		}
	}

	bIsLeftMouseClick = true;
	UnitControlHUD -> MarqueePressed();
}

void AOperatorPawn::OnMouseLeftCompleted(const FInputActionValue& Value)
{
	bIsLeftMouseClick = false;
	UnitControlHUD -> MarqueeReleased();
}


void AOperatorPawn::OnMouseRight(const FInputActionValue& Value)
{
	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController()))
	{
		FHitResult HitResult;
		PlayerController -> GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
		if(HitResult.bBlockingHit)
		{
			for(auto* Unit: SelectedUnits)
			{
				if(Unit != nullptr)
				{
					if(ASoldierAIController* RifleController = Cast<ASoldierAIController>(UAIBlueprintHelperLibrary::GetAIController(Unit)))
					{
						RifleController -> MoveCommand(HitResult.Location);
					}
				}
				
			}

			for(auto* Tank: SelectedTanks)
			{
				if(Tank != nullptr)
				{
					if(ATankAIController* TankAIController = Cast<ATankAIController>(Tank -> GetController()))
					{
						TankAIController -> MoveCommand(HitResult.Location);
					}
				}
			}
			if(MoveCursorEffect != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MoveCursorEffect, HitResult.Location, FRotator::ZeroRotator);
			}
		}
		
	}
}

void AOperatorPawn::OnMouseWheelUp(const FInputActionValue& Value)
{
	const float delta = Value.Get<float>();
	Camera -> FieldOfView = FMath::Clamp(Camera -> FieldOfView - delta * 4, 45.f, 90.f);
	ScrollSpeed = FMath::Clamp(ScrollSpeed - delta * 100, 6000.f, 120000.f);
}

void AOperatorPawn::OnMouseWheelDown(const FInputActionValue& Value)
{
	const float delta = Value.Get<float>();
	Camera -> FieldOfView = FMath::Clamp(Camera -> FieldOfView + delta * 4, 45.f, 90.f);
	ScrollSpeed = FMath::Clamp(ScrollSpeed + delta * 100, 6000.f, 120000.f);
}

void AOperatorPawn::OnSpawnSpectator(const FInputActionValue& Value)
{
	AOperatorSpectatorPawn* spawn = GetWorld() -> SpawnActor<AOperatorSpectatorPawn>(SpectatorClass, CurrentMousePosition, FRotator::ZeroRotator);
	if(SpectatorPawnArray.Num() == 3)
	{
		SpectatorPawnArray.RemoveAt(0);
	}
	SpectatorPawnArray.Add(spawn);
}

void AOperatorPawn::OnSwitchSlot1(const FInputActionValue& Value)
{
	AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController());
	
	if(PlayerController && SpectatorPawnArray.Num() > 0)
	{
		PlayerController -> Possess(SpectatorPawnArray[0]);
	}
}

void AOperatorPawn::OnSwitchSlot2(const FInputActionValue& Value)
{
	AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController());
	
	if(PlayerController && SpectatorPawnArray.Num() > 1)
	{
		PlayerController -> Possess(SpectatorPawnArray[1]);
	}
}

void AOperatorPawn::OnSwitchSlot3(const FInputActionValue& Value)
{
	AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController());
	
	if(PlayerController && SpectatorPawnArray.Num() > 2)
	{
		PlayerController -> Possess(SpectatorPawnArray[2]);
	}
}

void AOperatorPawn::OnAttackReady(const FInputActionValue& Value)
{
	bAttackReady = true;
}
