// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/Soldier.h"

#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/FlockingComponent.h"
#include "LDG/RifleSoliderAnimInstance.h"
#include "LDG/SoldierAIController.h"

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
	
	GetMesh() -> SetReceivesDecals(false);

	FlockingComponent = CreateDefaultSubobject<UFlockingComponent>(TEXT("FlockingComponent"));
}

// Called when the game starts or when spawned
void ASoldier::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UGameplayStatics::ApplyDamage(this, 1.f, GetInstigator() -> GetController(), this, UDamageType::StaticClass());
}

void ASoldier::Selected()
{
	bSelected = true;
	SelectedDecal -> SetVisibility(true);
}

void ASoldier::Deselected()
{
	bSelected = false;
	SelectedDecal -> SetVisibility(false);
}

float ASoldier::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
									   AController* EventInstigator, AActor* DamageCauser)
{
	//GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Take Damage"));
	Health -= DamageAmount;
	
	if(Health <= 0)	// Die
	{
		if(auto* con = Cast<ASoldierAIController>(UAIBlueprintHelperLibrary::GetAIController(this)))
		{
			con -> Die();

			FTimerHandle DieTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(DieTimerHandle, [this]()
			{
				Destroy();
			}, 3.0f, false, 3.0f);
		}
	}
	
	return DamageAmount;
}