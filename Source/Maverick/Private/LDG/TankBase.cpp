// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/TankBase.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/DecalComponent.h"
#include "LDG/TankAIController.h"

ATankBase::ATankBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SelectedDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectedDecal"));
	SelectedDecal -> SetupAttachment(RootComponent);
	SelectedDecal -> SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SelectedDecal -> DecalSize = FVector(400.f);
	SelectedDecal -> SetVisibility(false);
	
	GetMesh() -> SetReceivesDecals(false);
}

void ATankBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATankBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

float ATankBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Take Damage"));
	Health -= DamageAmount;
	
	if(Health <= 0)	// Die
	{
		if(auto* con = Cast<ATankAIController>(UAIBlueprintHelperLibrary::GetAIController(this)))
		{
			con -> SetState(ETankState::Die);
			//con -> GetRifleAnimInstance() -> PlayDeathMontage();
			GetMesh() -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//GetCapsuleComponent() -> SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FTimerHandle DieTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(DieTimerHandle, [this]()
			{
				Destroy();
			}, 3.0f, false, 3.0f);
		}
	}
	
	return DamageAmount;
}