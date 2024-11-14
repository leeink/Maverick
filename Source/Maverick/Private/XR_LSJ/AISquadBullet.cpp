// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadBullet.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/Soldier.h"
#include "LDG/SoldierAIController.h"
#include "LDG/TankBase.h"
#include "LDG/TankAIController.h"

// Sets default values
AAISquadBullet::AAISquadBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("EnemyBullet"));
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
	
	SetMovementComp(CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp")));
	GetMovementComp()->SetUpdatedComponent(RootComponent);
	GetMovementComp()->ProjectileGravityScale=0.1f;
}

void AAISquadBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{	
		 ASoldier* TargetPlayerUnit = Cast<ASoldier>(OtherActor);
		 if (TargetPlayerUnit)
		 {
			 ASoldierAIController* controller = Cast<ASoldierAIController>(TargetPlayerUnit->GetController());
			 if (controller && (controller->GetCurrentState() != EState::Die && !controller->IsDead()))
			 {
				if(GetOwner()!=nullptr)
					UGameplayStatics::ApplyDamage(OtherActor, 5.0f, GetOwner()->GetInstigatorController(),GetOwner(),NULL);
				else
					UGameplayStatics::ApplyDamage(OtherActor, 5.0f, nullptr,nullptr ,NULL);
			 }
		 }
		ATankBase* TargetPlayerTankUnit = Cast<ATankBase>(OtherActor);
		 if (TargetPlayerTankUnit)
		 {
			 ATankAIController* controller = Cast<ATankAIController>(TargetPlayerTankUnit->GetController());
			 if (controller && (controller->CurrentState != ETankState::Die))
			 {
				if(GetOwner()!=nullptr)
					UGameplayStatics::ApplyDamage(OtherActor, 1.0f, GetOwner()->GetInstigatorController(),GetOwner(),NULL);
				else
					UGameplayStatics::ApplyDamage(OtherActor, 1.0f, nullptr,nullptr ,NULL);
			 }
		 }
	}
	else if (OtherActor && OtherActor->ActorHasTag("Enemy"))
	{	
		//UGameplayStatics::ApplyDamage(OtherActor, 30.0f, GetOwner()->GetInstigatorController(),GetOwner(),NULL);
	}
	DestroyBullet();
}

// Called when the game starts or when spawned
void AAISquadBullet::BeginPlay()
{
	Super::BeginPlay();
	BulletFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BulletFXSystem,MeshComp,"", FVector::ZeroVector, FRotator::ZeroRotator, FVector(1, 1, 1), EAttachLocation::SnapToTarget, true, ENCPoolMethod::AutoRelease);
	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &AAISquadBullet::DestroyBullet, 1.0f, true);

}
void AAISquadBullet::DestroyBullet()
{
	if(BulletFXComponent)
		BulletFXComponent->Deactivate();
	Destroy();
}

void AAISquadBullet::InitMovement(FVector Direction)
{
	GetMovementComp()->InitialSpeed = 5000;
	GetMovementComp()->MaxSpeed = 5000;
	GetMovementComp()->Velocity = GetMovementComp()->InitialSpeed * Direction;
}

// Called every frame
void AAISquadBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorRelativeLocation(+GetActorForwardVector()*100.0f*DeltaTime);
}

