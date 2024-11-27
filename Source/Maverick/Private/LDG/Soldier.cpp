// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/Soldier.h"

#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LDG/ArmyWidgetBase.h"
#include "LDG/FlockingComponent.h"
#include "LDG/OperatorPawn.h"
#include "LDG/RifleSoliderAnimInstance.h"
#include "LDG/SoldierAIController.h"
#include "LDG/SoldierHealthWidget.h"
#include "XR_LSJ/EnemyManager.h"

// Sets default values
ASoldier::ASoldier()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SelectedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectedDecal"));
	SelectedDecal -> SetupAttachment(RootComponent);
	SelectedDecal -> SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SelectedDecal -> DecalSize = FVector(128.f);
	SelectedDecal -> SetVisibility(false);

	ArmyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ArmyWidget"));
	ArmyWidget -> SetupAttachment(RootComponent);
	ArmyWidget -> SetWidgetSpace(EWidgetSpace::Screen);
	ArmyWidget -> SetDrawSize(FVector2D(100.f, 100.f));
	ArmyWidget -> SetRelativeLocation(FVector(0.f,0.f,155.f));
	ArmyWidget -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmyWidget -> SetVisibility(false);

	MiniMapWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MiniMapWidget"));
	MiniMapWidget -> SetupAttachment(RootComponent);
	MiniMapWidget -> SetWidgetSpace(EWidgetSpace::World);
	MiniMapWidget -> SetRelativeLocation(FVector(0.f,0.f,10000.f));
	MiniMapWidget -> SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	MiniMapWidget -> SetRelativeScale3D(FVector(7.f));
	MiniMapWidget -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MiniMapWidget -> CastShadow = false;
	
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget -> SetupAttachment(RootComponent);
	HealthWidget -> SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidget -> SetDrawSize(FVector2D(160.f, 160.f));
	HealthWidget -> SetRelativeLocation(FVector(0.f,0.f,155.f));
	HealthWidget -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidget -> SetVisibility(false);
	
	GetMesh() -> SetReceivesDecals(false);

	FlockingComponent = CreateDefaultSubobject<UFlockingComponent>(TEXT("FlockingComponent"));
}

// Called when the game starts or when spawned
void ASoldier::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
	
	ArmyWidgetInstance = Cast<UArmyWidgetBase>(ArmyWidget -> GetUserWidgetObject());
	SoldierHealthWidgetInstance = Cast<USoldierHealthWidget>(HealthWidget -> GetUserWidgetObject());
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UGameplayStatics::ApplyDamage(this, 1.f, GetInstigator() -> GetController(), this, UDamageType::StaticClass());
	ArmyWidgetBilboard();
}

void ASoldier::Selected()
{
	bSelected = true;
	SelectedDecal -> SetVisibility(true);
	HealthWidget -> SetVisibility(true);
}

void ASoldier::Deselected()
{
	bSelected = false;
	SelectedDecal -> SetVisibility(false);
	HealthWidget -> SetVisibility(false);
}

void ASoldier::ToggleWidget(bool bShow)
{
	bShow ? ArmyWidget -> SetVisibility(true) : ArmyWidget -> SetVisibility(false);
}

void ASoldier::ArmyWidgetBilboard()
{
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(this -> GetActorLocation(),
		GetWorld() -> GetFirstPlayerController() -> GetPawn() -> GetActorLocation());
	
	ArmyWidget -> SetWorldRotation(NewRotation);
	HealthWidget -> SetWorldRotation(NewRotation);
}

float ASoldier::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                           AController* EventInstigator, AActor* DamageCauser)
{
	//GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Take Damage"));
	Health -= DamageAmount;
	SoldierHealthWidgetInstance -> UpdateHealth(Health / MaxHealth);
	
	if(Health <= 0)	// Die
	{
		if(auto* con = Cast<ASoldierAIController>(UAIBlueprintHelperLibrary::GetAIController(this)))
		{
			con -> Die();
			con -> GetRifleAnimInstance() -> Montage_Stop(.25f);
			if(Del_PlayerSoldierUnitDie.IsBound())
			{
				Del_PlayerSoldierUnitDie.Execute();
			}

			FTimerHandle DieTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(DieTimerHandle, [this]()
			{
				if(DieSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), DieSound, GetActorLocation());
				}
				
				Destroy();
			}, 3.0f, false, 2.f);
		}
	}
	
	return DamageAmount;
}
