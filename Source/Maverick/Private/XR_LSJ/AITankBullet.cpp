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
	BulletEffectComp->Deactivate();             // ��ƼŬ Ȱ��ȭ

	BulletFXComponent= CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletFXComponent")); 
	FVector NewScale =   (FVector(1.0f, 1.0f, 1.0f) * 10.0f);
	BulletFXComponent->SetRelativeScale3D(NewScale);
	BulletFXComponent->SetupAttachment(RootComponent);

	ExplosiveMaxDamage = 500.0f;
	ExplosiveMinDamage = 200.0f;
	ExplosiveRange = 100.0f;
	
	MovementComp = (CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp")));
	MovementComp->bRotationFollowsVelocity = true;
}

void AAITankBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//float BaseDamage = 50.f;       // �߽ɿ����� �ִ� ������
 //   float MinimumDamage = 10.f;     // �ִ� �ݰ濡���� �ּ� ������
 //   float DamageInnerRadius = 100.f; // �ִ� �������� ����Ǵ� �ݰ�
 //   float DamageOuterRadius = 400.f; // �ּ� �������� ����Ǵ� �ݰ�
 //   float DamageFalloff = 1.0f;     // �������� �پ��� �ӵ� (1.0f = ���� ����)
	//TArray<AActor*> ActorsToIgnore;
	//ActorsToIgnore.Add(this);
	//ActorsToIgnore.Add(GetOwner());
 //   // �ݰ� ���� ���͵鿡�� �Ÿ� ��� �������� ����
 //   UGameplayStatics::ApplyRadialDamageWithFalloff(
 //       GetWorld(),
 //       BaseDamage,
 //       MinimumDamage,
 //       GetActorLocation(),
 //       DamageInnerRadius,
 //       DamageOuterRadius,
 //       DamageFalloff,
 //       UDamageType::StaticClass(),
 //       ActorsToIgnore, // ������ ���͵�
 //       GetOwner(),
 //       GetOwner()->GetInstigatorController()
 //   );

	FVector Start = GetActorLocation();
	FVector End = GetActorLocation();
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
	TArray<FHitResult> OutHits;
	bool bIgnoreSelf = true;
	FLinearColor TraceColor = FLinearColor::Gray;
	FLinearColor TraceHitColor = FLinearColor::Blue;
	float DrawTime = 1.0f;
	//DrawDebugSphere(GetWorld(), Start, 10.0f, 12, FColor::Red, false, 5.0f);

	const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,ExplosiveRange,TraceChannel,bTraceComplex,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf,TraceColor,TraceHitColor);
	if (Hit)
	{
		//DrawDebugSphere(GetWorld(),Start, ExplosiveRange, 12, FColor::Black, false, 5.0f);
		for(FHitResult& HitResult :OutHits)
		{
			if (HitResult.GetActor() && HitResult.GetActor()->ActorHasTag("Player"))
			{			
				Start.Z = 0;
				FVector TargetLocation = HitResult.GetActor()->GetActorLocation();
				TargetLocation.Z=0;
				float Distance = FVector::Distance(Start,TargetLocation );
				float Damage = FMath::Lerp(ExplosiveMinDamage, ExplosiveMaxDamage, (ExplosiveRange-Distance) / ExplosiveRange);
				Damage = 100.0f;
				UE_LOG(LogTemp,Error,TEXT("%f %f %f"),ExplosiveMinDamage, ExplosiveMaxDamage, (ExplosiveRange-Distance) / ExplosiveRange);
				if(GetOwner()!=nullptr)
					UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, GetOwner()->GetInstigatorController(), GetOwner(),NULL);
				else
					UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, nullptr,nullptr ,NULL);
				//UE_LOG(LogTemp,Error,TEXT("%s HitResult"),*HitResult.GetComponent()->GetName());
				UE_LOG(LogTemp,Error,TEXT("%f"),Damage);
				
			}
		}
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
        BulletEffectComp->SetTemplate(BulletEffect);  // ��ƼŬ ���ø� ����
		// ��ƼŬ�� ����� �� ȣ��� ��������Ʈ ���ε�
        BulletEffectComp->OnSystemFinished.AddDynamic(this,&AAITankBullet::OnParticleSystemFinished);
    }
	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &AAITankBullet::DestroyBullet, 10.0f, true);
}

void AAITankBullet::OnParticleSystemFinished(UParticleSystemComponent* FinishedComponent)
{
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

