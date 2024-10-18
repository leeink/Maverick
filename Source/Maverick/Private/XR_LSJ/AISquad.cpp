// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "XR_LSJ/AISquadController.h"

// Sets default values
AAISquad::AAISquad()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FSMComp = CreateDefaultSubobject<UAISquadFSMComponent>(TEXT("FSMComp"));

	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	if (nullptr == GunMesh)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshFinder(TEXT("/Game/Asset/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SK_KA47.SK_KA47"));
		if (GunMeshFinder.Succeeded())
		{
			GunMeshComp->SetSkeletalMesh(GunMeshFinder.Object);
		}
	}
	else
		GunMeshComp->SetSkeletalMesh(GunMesh);
	GunMeshComp->SetupAttachment(GetMesh(),FName("RightHandThumb4"));
	GunMeshComp->SetRelativeLocation(FVector(-3.300448,1.447269,2.664125));
	GunMeshComp->SetRelativeRotation(FRotator(47.178474,38.000000,190.163415));

	AIControllerClass = AAISquadController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAISquad::BeginPlay()
{
	Super::BeginPlay();

}
void AAISquad::TestMove()
{
	
	FSMComp->SetArrivalPoint(TestMovePoint);
	TestMovePoint*=(-1);
	FSMComp->SetState(EEnemyState::MOVE);
}


// Called every frame
void AAISquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAISquad::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

