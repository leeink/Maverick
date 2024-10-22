// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "XR_LSJ/AISquadController.h"
#include "NiagaraFunctionLibrary.h" 
#include "NiagaraSystem.h" 
#include "NiagaraComponent.h" 
#include "XR_LSJ/AISquadBullet.h"
#include "Engine/World.h"

// Sets default values
AAISquad::AAISquad()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FSMComp = CreateDefaultSubobject<UAISquadFSMComponent>(TEXT("FSMComp"));
	
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshFinder(TEXT("/Game/Asset/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SK_KA47.SK_KA47"));
	if (GunMeshFinder.Succeeded())
	{
		GunMeshComp->SetSkeletalMesh(GunMeshFinder.Object);
	}
	GunMeshComp->SetupAttachment(GetMesh(),FName("RightHandThumb4"));
	GunMeshComp->SetRelativeLocation(FVector(-3.300448,1.447269,2.664125));
	GunMeshComp->SetRelativeRotation(FRotator(47.178474,38.000000,190.163415));
	
	AIControllerClass = AAISquadController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAISquad::SpawnBullet()
{

}

void AAISquad::AttackFire()
{
	//muzzle ÀÌÆåÆ® 
	GunMuzzleFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(GunMuzzleFXSystem,GunMeshComp,TEXT("Muzzle"),FVector::ZeroVector,FRotator::ZeroRotator,FVector(1,1,1),EAttachLocation::SnapToTarget,true,ENCPoolMethod::AutoRelease);
	//ÃÑ¾Ë ¼ÒÈ¯
	GetWorld()->SpawnActor<AAISquadBullet>(BulletFactory,GunMeshComp->GetSocketLocation(TEXT("Muzzle")),GunMeshComp->GetSocketRotation(TEXT("Muzzle")));
	UE_LOG(LogTemp, Warning, TEXT("%s"),*GunMeshComp->GetSocketLocation(TEXT("Muzzle")).ToString());
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

