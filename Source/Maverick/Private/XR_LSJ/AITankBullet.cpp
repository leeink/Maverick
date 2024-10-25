// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AITankBullet.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AAITankBullet::AAITankBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("EnemyBullet"));
	CollisionComp->SetSphereRadius(13);
	CollisionComp->SetCanEverAffectNavigation(false);
	SetRootComponent(CollisionComp);

	BulletEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletEffectComp")); 
	BulletEffectComp->SetupAttachment(RootComponent);
	
	MovementComp = (CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp")));
}
void AAITankBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor && OtherActor->ActorHasTag("Test"))
	{	
		//UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	}
	else if (OtherActor && OtherActor->ActorHasTag("Enemy"))
	{	
		UGameplayStatics::ApplyDamage(OtherActor, 30.0f, GetOwner()->GetInstigatorController(), this, NULL);
	}
	DestroyBullet();
}
// Called when the game starts or when spawned
void AAITankBullet::BeginPlay()
{
	Super::BeginPlay();
	if (BulletEffect)
    {
        BulletEffectComp->SetTemplate(BulletEffect);  // 파티클 템플릿 설정
        BulletEffectComp->Activate(true);               // 파티클 활성화
    }
	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &AAITankBullet::DestroyBullet, 10.0f, true);
}

void AAITankBullet::DestroyBullet()
{
	if (BulletEffectComp)
		BulletEffectComp->Deactivate();
	Destroy();
}
void AAITankBullet::InitMovement(FVector Direction)
{
	MovementComp->InitialSpeed = 5000;
	MovementComp->MaxSpeed = 5000;
	MovementComp->Velocity = MovementComp->InitialSpeed * Direction;
}
// Called every frame
void AAITankBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

