// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AITankBullet.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
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
	BulletEffectComp->Deactivate();             // 파티클 활성화

	BulletFXComponent= CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletFXComponent")); 
	FVector NewScale =   (FVector(1.0f, 1.0f, 1.0f) * 10.0f);
	BulletFXComponent->SetRelativeScale3D(NewScale);
	BulletFXComponent->SetupAttachment(RootComponent);

	ExplosiveDamage = 30.0f;
	ExplosiveRange = 100.0f;
	
	MovementComp = (CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp")));
	MovementComp->bRotationFollowsVelocity = true;
}
void AAITankBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FVector Start = GetActorLocation();
	FVector End = GetActorLocation();
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::ForOneFrame;
	TArray<FHitResult> OutHits;
	bool bIgnoreSelf = true;
	FLinearColor TraceColor = FLinearColor::Gray;
	FLinearColor TraceHitColor = FLinearColor::Blue;
	float DrawTime = 1.0f;
	
	//SquadManagerAbility.FindTargetRange 범위 탐색
	const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,ExplosiveRange,TraceChannel,bTraceComplex,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf,TraceColor,TraceHitColor);
	if (Hit)
	{
		for(FHitResult HitResult :OutHits)
		{
			if (OtherActor && OtherActor->ActorHasTag("Enemy"))
			{			
				UGameplayStatics::ApplyDamage(OtherActor, ExplosiveDamage, GetOwner()->GetInstigatorController(), GetOwner(),NULL);
			}
		}
	}

	if(OtherActor==GetOwner())
		return;
	if (OtherActor && OtherActor->ActorHasTag("Test"))
	{	
		//UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	}
	else if (OtherActor && OtherActor->ActorHasTag("Enemy"))
	{	
		
	}
	if(BulletFXComponent)
		BulletFXComponent->Deactivate();
	MovementComp->StopMovementImmediately();
	BulletEffectComp->Activate(true);
	//DestroyBullet();
}
// Called when the game starts or when spawned
void AAITankBullet::BeginPlay()
{
	Super::BeginPlay();

	//BulletFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BulletFXSystem,CollisionComp,"", FVector::ZeroVector, GetActorRotation(), FVector(1, 1, 1), EAttachLocation::SnapToTarget, true, ENCPoolMethod::AutoRelease);

	if (BulletEffect)
    {
        BulletEffectComp->SetTemplate(BulletEffect);  // 파티클 템플릿 설정
		// 파티클이 종료될 때 호출될 델리게이트 바인딩
        BulletEffectComp->OnSystemFinished.AddDynamic(this,&AAITankBullet::OnParticleSystemFinished);
    }
	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &AAITankBullet::DestroyBullet, 10.0f, true);
}

void AAITankBullet::OnParticleSystemFinished(UParticleSystemComponent* FinishedComponent)
{
    UE_LOG(LogTemp, Log, TEXT("Particle System Finished"));
	DestroyBullet();
}
void AAITankBullet::DestroyBullet()
{
	if(BulletFXComponent)
		BulletFXComponent->Deactivate();
	if (BulletEffect)
		BulletEffectComp->Deactivate();
	Destroy();
}
void AAITankBullet::InitMovement(FVector Direction)
{
	MovementComp->InitialSpeed = 50014.769531;
	MovementComp->MaxSpeed = 50014.769531;
	MovementComp->Velocity = Direction;//MovementComp->InitialSpeed * Direction;
}
// Called every frame
void AAITankBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

