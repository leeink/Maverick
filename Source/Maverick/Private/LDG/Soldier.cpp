// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/Soldier.h"

#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/DecalComponent.h"

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
}

// Called when the game starts or when spawned
void ASoldier::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoldier::Wait()
{
	GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Wait"));
}

void ASoldier::Move(FVector GoalLocation)
{
	GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move"));
	UAIBlueprintHelperLibrary::GetAIController(this) -> StopMovement();
	UAIBlueprintHelperLibrary::GetAIController(this) -> MoveToLocation(GoalLocation);
}

void ASoldier::Attack()
{
	GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack"));
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

