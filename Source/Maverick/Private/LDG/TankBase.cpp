// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/TankBase.h"

#include "NavigationInvokerComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LDG/ArmyWidgetBase.h"
#include "LDG/OperatorPawn.h"
#include "LDG/TankAIController.h"

ATankBase::ATankBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh -> SetupAttachment(Collision);
	
	SelectedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectedDecal"));
	SelectedDecal -> SetupAttachment(Mesh);
	SelectedDecal -> SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SelectedDecal -> DecalSize = FVector(400.f);
	SelectedDecal -> SetVisibility(false);

	ArmyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ArmyWidget"));
	ArmyWidget -> SetupAttachment(RootComponent);
	ArmyWidget -> SetWidgetSpace(EWidgetSpace::Screen);
	ArmyWidget -> SetDrawSize(FVector2D(200.f, 200.f));
	ArmyWidget -> SetRelativeLocation(FVector(0.f,0.f,500.f));
	ArmyWidget -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmyWidget -> SetVisibility(true);

	MiniMapWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MiniMapWidget"));
	MiniMapWidget -> SetupAttachment(RootComponent);
	MiniMapWidget -> SetWidgetSpace(EWidgetSpace::World);
	MiniMapWidget -> SetRelativeLocation(FVector(0.f,0.f,10000.f));
	MiniMapWidget -> SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	MiniMapWidget -> SetRelativeScale3D(FVector(8.f));
	MiniMapWidget -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MiniMapWidget -> CastShadow = false;
	
	GetMesh() -> SetReceivesDecals(false);

	NavigationInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvoker"));
	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
}

void ATankBase::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	ArmyWidgetInstance = Cast<UArmyWidgetBase>(ArmyWidget -> GetUserWidgetObject());
}

void ATankBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ArmyWidgetBilboard();
	OnRotate(Cast<ATankAIController>(UAIBlueprintHelperLibrary::GetAIController(this)) -> GetRotationAngle());
	//UGameplayStatics::ApplyDamage(this, 1000.f, GetInstigatorController(), this, nullptr);
}

void ATankBase::Selected()
{
	bSelected = true;
	SelectedDecal -> SetVisibility(true);
}

void ATankBase::Deselected()
{
	bSelected = false;
	SelectedDecal -> SetVisibility(false);
}

void ATankBase::ArmyWidgetBilboard()
{
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(this -> GetActorLocation(),
		GetWorld() -> GetFirstPlayerController() -> GetPawn() -> GetActorLocation());

	ArmyWidget -> SetWorldRotation(NewRotation);
}

float ATankBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                            AActor* DamageCauser)
{
	//GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Take Damage"));
	Health -= DamageAmount;

	ArmyWidgetInstance -> UpdateHealthBar(Health / MaxHealth);
	
	if(Health <= 0)	// Die
	{
		if(auto* con = Cast<ATankAIController>(UAIBlueprintHelperLibrary::GetAIController(this)))
		{
			con -> SetState(ETankState::Die);
			//con -> GetRifleAnimInstance() -> PlayDeathMontage();
			GetMesh() -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//GetCapsuleComponent() -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if(Del_PlayerTankUnitDie.IsBound())
			{
				Del_PlayerTankUnitDie.Execute();
			}

			GetWorld() -> SpawnActor<AActor>(DamagedMesh, GetActorLocation(), GetActorRotation());
			if(DestroyEffect != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation());
			}

			if(DestroySound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation());
			}
			/*
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Pre Number: %d"), Cast<AOperatorPawn>(GetWorld() -> GetFirstPlayerController() -> GetPawn()) -> GetSelectedTanks().Num()));
			Cast<AOperatorPawn>(GetWorld() -> GetFirstPlayerController() -> GetPawn()) -> GetSelectedTanks().Remove(this);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Post Number: %d"), Cast<AOperatorPawn>(GetWorld() -> GetFirstPlayerController() -> GetPawn()) -> GetSelectedTanks().Num()));
			*/
			Destroy();
		}
	}
	
	return DamageAmount;
}
