// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadBullet.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Components/SphereComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
AAISquadBullet::AAISquadBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComp->SetSphereRadius(13);
	CollisionComp->SetCanEverAffectNavigation(false);
	SetRootComponent(CollisionComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp")); 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletFinder(TEXT("/Script/Engine.StaticMesh'/Game/Asset/sA_Megapack_v1/sA_ShootingVfxPack/Meshes/SM_Bullet_1.SM_Bullet_1'"));
	if (BulletFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(BulletFinder.Object);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetupAttachment(RootComponent);
	}
	
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->SetUpdatedComponent(RootComponent);
	MovementComp->InitialSpeed = 5000;
	MovementComp->MaxSpeed = 5000;
	MovementComp->bShouldBounce = true;
	MovementComp->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void AAISquadBullet::BeginPlay()
{
	Super::BeginPlay();
	BulletFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BulletFXSystem,MeshComp,"", FVector::ZeroVector, FRotator::ZeroRotator, FVector(1, 1, 1), EAttachLocation::SnapToTarget, true, ENCPoolMethod::AutoRelease);

}

// Called every frame
void AAISquadBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorRelativeLocation(+GetActorForwardVector()*100.0f*DeltaTime);
}

