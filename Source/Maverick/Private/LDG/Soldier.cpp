// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/Soldier.h"

// Sets default values
ASoldier::ASoldier()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ASoldier::Move()
{
	GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move"));
}

void ASoldier::Attack()
{
	GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack"));
}

