// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/OperatorPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/ArmySelectWidget.h"
#include "LDG/OperatorPlayerController.h"
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
	
	Collsion = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collsion);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera -> SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));

	LocationArray.Init(FVector(0.f), 3);
}

// Called when the game starts or when spawned
void AOperatorPawn::BeginPlay()
{
	Super::BeginPlay();
	
	UnitControlHUD = Cast<AUnitControlHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0) -> GetHUD());
	ArmySelectWidget = Cast<UArmySelectWidget>(CreateWidget(GetWorld(), ArmySelectWidgetClass));
	ArmySelectWidget -> AddToViewport();
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
		EnhancedInputComponent->BindAction(IA_SwitchSlot1 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot1);
		EnhancedInputComponent->BindAction(IA_SwitchSlot2 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot2);
		EnhancedInputComponent->BindAction(IA_SwitchSlot3 , ETriggerEvent::Started , this , &AOperatorPawn::OnSwitchSlot3);
		EnhancedInputComponent->BindAction(IA_AttackReady , ETriggerEvent::Started , this , &AOperatorPawn::OnAttackReady);
		EnhancedInputComponent->BindAction(IA_Ctrl , ETriggerEvent::Triggered , this , &AOperatorPawn::OnCtrlTriggered);
		EnhancedInputComponent->BindAction(IA_Ctrl , ETriggerEvent::Completed , this , &AOperatorPawn::OnCtrlReleased);
		EnhancedInputComponent->BindAction(IA_ArmySlot1 , ETriggerEvent::Started , this , &AOperatorPawn::OnSelectSlot1);
		EnhancedInputComponent->BindAction(IA_ArmySlot2 , ETriggerEvent::Started , this , &AOperatorPawn::OnSelectSlot2);
		EnhancedInputComponent->BindAction(IA_ArmySlot3 , ETriggerEvent::Started , this , &AOperatorPawn::OnSelectSlot3);
		EnhancedInputComponent->BindAction(IA_StopAction, ETriggerEvent::Started, this, &AOperatorPawn::OnStopAction);
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
				if(Unit != nullptr)
				{
					if(ASoldierAIController* SoldierController = Cast<ASoldierAIController>(Unit -> GetController()))
					{
						SoldierController -> ChaseCommand(HitResult.Location);
					}
				}
				
			}

			for(auto* Tank: SelectedTanks)
			{
				if(Tank != nullptr)
				{
					if(ATankAIController* TankAIController = Cast<ATankAIController>(Tank -> GetController()))
					{
						//GEngine -> AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Chase Command"));
						TankAIController -> ChaseCommand(HitResult.Location);
					}
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

void AOperatorPawn::OnMouseRightMinimap(FVector Location)
{
	if(AOperatorPlayerController* PlayerController = Cast<AOperatorPlayerController>(GetController()))
	{
		for(auto* Unit: SelectedUnits)
		{
			if(Unit != nullptr)
			{
				if(ASoldierAIController* RifleController = Cast<ASoldierAIController>(UAIBlueprintHelperLibrary::GetAIController(Unit)))
				{
					RifleController -> MoveCommand(Location);
				}
			}
				
		}
		for(auto* Tank: SelectedTanks)
		{
			if(Tank != nullptr)
			{
				if(ATankAIController* TankAIController = Cast<ATankAIController>(Tank -> GetController()))
				{
					TankAIController -> MoveCommand(Location);
				}
			}
		}
	}
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

void AOperatorPawn::OnSwitchSlot1(const FInputActionValue& Value)
{
	if(bCtrlPressed)
	{
		LocationArray[0] = GetActorLocation();
	}
	else
	{
		SetActorLocation(LocationArray[0], false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AOperatorPawn::OnSwitchSlot2(const FInputActionValue& Value)
{
	if(bCtrlPressed)
	{
		LocationArray[1] = GetActorLocation();
	}
	else
	{
		SetActorLocation(LocationArray[1], false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AOperatorPawn::OnSwitchSlot3(const FInputActionValue& Value)
{
	if(bCtrlPressed)
	{
		LocationArray[2] = GetActorLocation();
	}
	else
	{
		SetActorLocation(LocationArray[2], false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AOperatorPawn::OnAttackReady(const FInputActionValue& Value)
{
	bAttackReady = true;
}

void AOperatorPawn::OnCtrlTriggered(const FInputActionValue& Value)
{
	bCtrlPressed = true;
}

void AOperatorPawn::OnCtrlReleased(const FInputActionValue& Value)
{
	bCtrlPressed = false;
}


void AOperatorPawn::OnSelectSlot1(const FInputActionValue& Value)
{
	// Unit designation
	if(bCtrlPressed)
	{
		if(SelectedUnits.Num() > 0)
		{
			ArmySlot1.Reset();
		}
		for(auto* Unit: SelectedUnits)
		{
			if(Unit != nullptr)
			{
				ArmySlot1.Add(Unit);
			}
		}
		for(auto* Tank: SelectedTanks)
		{
			if(Tank != nullptr)
			{
				ArmySlot1.Add(Tank);
			}
		}
	}
	else
	{
		float currentTime = GetWorld() -> GetTimeSeconds();
		if(bWaitingForSecondClick)	// Double Click
		{
			float timeBetweenClicks = currentTime - LastClickTime;

			if(timeBetweenClicks < DoubleClickInterval)
			{
				FVector ArmyLocation = ArmySlot1[FMath::RandHelper(ArmySlot1.Num() - 1)] -> GetActorLocation();
				DoubleClickMoveLocation(ArmyLocation);
			}
			bWaitingForSecondClick = false;
		}
		else      // Single Click
		{
			if(ArmySlot1.Num() > 0)
			{
				for(auto* Unit: SelectedUnits)
				{
					if(Unit != nullptr)
					{
						Unit -> Deselected();
					}
				}
				for(auto* Tank: SelectedTanks)
				{
					if(Tank != nullptr)
					{
						Tank -> Deselected();
					}
				}
				SelectedUnits.Reset();
				SelectedTanks.Reset();
		
				for(auto* Unit: ArmySlot1)
				{
					if(Unit != nullptr)
					{
						if(ASoldier* Soldier = Cast<ASoldier>(Unit))
						{
							SelectedUnits.Add(Soldier);
							Soldier -> Selected();
						}
						else if(ATankBase* Tank = Cast<ATankBase>(Unit))
						{
							SelectedTanks.Add(Tank);
							Tank -> Selected();
						}
					}
				}
				
				bWaitingForSecondClick = true;
				LastClickTime = currentTime;
        
				// Reset Timer
				FTimerHandle ResetTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, [this]()
				{
					bWaitingForSecondClick = false;
				}, DoubleClickInterval, false);

				ArmySelectWidget -> ActivationWidget(0);
			}
		}
		
	}
}

void AOperatorPawn::OnSelectSlot2(const FInputActionValue& Value)
{
	// Unit designation
	if(bCtrlPressed)
	{
		if(SelectedUnits.Num() > 0)
		{
			ArmySlot2.Reset();
		}
		for(auto* Unit: SelectedUnits)
		{
			if(Unit != nullptr)
			{
				ArmySlot2.Add(Unit);
			}
		}
		for(auto* Tank: SelectedTanks)
		{
			if(Tank != nullptr)
			{
				ArmySlot2.Add(Tank);
			}
		}
	}
	else
	{
		float currentTime = GetWorld() -> GetTimeSeconds();
		if(bWaitingForSecondClick)	// Double Click
		{
			float timeBetweenClicks = currentTime - LastClickTime;

			if(timeBetweenClicks < DoubleClickInterval)
			{
				FVector ArmyLocation = ArmySlot2[FMath::RandHelper(ArmySlot2.Num() - 1)] -> GetActorLocation();
				DoubleClickMoveLocation(ArmyLocation);
			}
			bWaitingForSecondClick = false;
		}
		else      // Single Click
		{
			if(ArmySlot2.Num() > 0)
			{
				for(auto* Unit: SelectedUnits)
				{
					if(Unit != nullptr)
					{
						Unit -> Deselected();
					}
				}
				for(auto* Tank: SelectedTanks)
				{
					if(Tank != nullptr)
					{
						Tank -> Deselected();
					}
				}
				SelectedUnits.Reset();
				SelectedTanks.Reset();
		
				for(auto* Unit: ArmySlot2)
				{
					if(Unit != nullptr)
					{
						if(ASoldier* Soldier = Cast<ASoldier>(Unit))
						{
							SelectedUnits.Add(Soldier);
							Soldier -> Selected();
						}
						else if(ATankBase* Tank = Cast<ATankBase>(Unit))
						{
							SelectedTanks.Add(Tank);
							Tank -> Selected();
						}
					}
				}
				
				bWaitingForSecondClick = true;
				LastClickTime = currentTime;
        
				// Reset Timer
				FTimerHandle ResetTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, [this]()
				{
					bWaitingForSecondClick = false;
				}, DoubleClickInterval, false);
				
				ArmySelectWidget -> ActivationWidget(1);
			}
		}
		
	}
}

void AOperatorPawn::OnSelectSlot3(const FInputActionValue& Value)
{
	// Unit designation
	if(bCtrlPressed)
	{
		if(SelectedUnits.Num() > 0)
		{
			ArmySlot3.Reset();
		}
		for(auto* Unit: SelectedUnits)
		{
			if(Unit != nullptr)
			{
				ArmySlot3.Add(Unit);
			}
		}
		for(auto* Tank: SelectedTanks)
		{
			if(Tank != nullptr)
			{
				ArmySlot3.Add(Tank);
			}
		}
	}
	else
	{
		float currentTime = GetWorld() -> GetTimeSeconds();
		if(bWaitingForSecondClick)	// Double Click
		{
			float timeBetweenClicks = currentTime - LastClickTime;

			if(timeBetweenClicks < DoubleClickInterval)
			{
				FVector ArmyLocation = ArmySlot3[FMath::RandHelper(ArmySlot3.Num() - 1)] -> GetActorLocation();
				DoubleClickMoveLocation(ArmyLocation);
			}
			bWaitingForSecondClick = false;
		}
		else      // Single Click
		{
			if(ArmySlot3.Num() > 0)
			{
				for(auto* Unit: SelectedUnits)
				{
					if(Unit != nullptr)
					{
						Unit -> Deselected();
					}
				}
				for(auto* Tank: SelectedTanks)
				{
					if(Tank != nullptr)
					{
						Tank -> Deselected();
					}
				}
				SelectedUnits.Reset();
				SelectedTanks.Reset();
		
				for(auto* Unit: ArmySlot3)
				{
					if(Unit != nullptr)
					{
						if(ASoldier* Soldier = Cast<ASoldier>(Unit))
						{
							SelectedUnits.Add(Soldier);
							Soldier -> Selected();
						}
						else if(ATankBase* Tank = Cast<ATankBase>(Unit))
						{
							SelectedTanks.Add(Tank);
							Tank -> Selected();
						}
					}
				}
				
				bWaitingForSecondClick = true;
				LastClickTime = currentTime;
        
				// Reset Timer
				FTimerHandle ResetTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, [this]()
				{
					bWaitingForSecondClick = false;
				}, DoubleClickInterval, false);

				ArmySelectWidget -> ActivationWidget(2);
			}
		}
		
	}
}

void AOperatorPawn::OnStopAction(const FInputActionValue& Value)
{
	for(auto* Unit: SelectedUnits)
	{
		if(Unit != nullptr)
		{
			if(ASoldierAIController* RifleController = Cast<ASoldierAIController>(UAIBlueprintHelperLibrary::GetAIController(Unit)))
			{
				RifleController -> IdleCommand();
			}
		}
	}
	for(auto* Tank: SelectedTanks)
	{
		if(Tank != nullptr)
		{
			if(ATankAIController* TankAIController = Cast<ATankAIController>(Tank -> GetController()))
			{
				TankAIController -> IdleCommand();
			}
		}
	}
}

void AOperatorPawn::DoubleClickMoveLocation(FVector Location)
{
	//GEngine -> AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Double Click Move"));
	SetActorLocation(FVector(Location.X - 1000.f, Location.Y - 1000.f, GetActorLocation().Z), false, nullptr, ETeleportType::TeleportPhysics);
}
