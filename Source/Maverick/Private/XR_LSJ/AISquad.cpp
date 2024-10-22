// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "XR_LSJ/AISquadController.h"
#include "NiagaraFunctionLibrary.h" 
#include "NiagaraSystem.h" 
#include "NiagaraComponent.h" 
#include "XR_LSJ/AISquadBullet.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"

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
	GunMeshComp->SetRelativeLocation(FVector(-3.612750,1.138105,3.562393));
	GunMeshComp->SetRelativeRotation(FRotator(23.987173,20.151969,-190.523942));

	AIControllerClass = AAISquadController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAISquad::SpawnBullet()
{

}

void AAISquad::AttackFire()
{
	//muzzle 이펙트 
	GunMuzzleFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(GunMuzzleFXSystem,GunMeshComp,TEXT("Muzzle"),FVector::ZeroVector,FRotator::ZeroRotator,FVector(1,1,1),EAttachLocation::SnapToTarget,true,ENCPoolMethod::AutoRelease);
	//총알 소환
	AAISquadBullet* Bullet = GetWorld()->SpawnActor<AAISquadBullet>(BulletFactory,GunMeshComp->GetSocketLocation(TEXT("Muzzle")),GunMeshComp->GetSocketRotation(TEXT("Muzzle")));
	if (Bullet)
	{
		Bullet->InitMovement(GunMeshComp->GetRightVector());
	}
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
float AAISquad::LookTarget(FVector TargetLocation)
{
	//	 //회전된 방향 계산
	//FVector Forward = AISquadBody()->GetActorForwardVector();
	//Forward.Z = 0;
	//Forward.Normalize();
	//FVector LeftDirection = Forward.RotateAngleAxis(-30.0f, FVector::UpVector);
	//FVector RightDirection = Forward.RotateAngleAxis(30.0f, FVector::UpVector);

	////두 벡터 사이의 각도
	//float Dot = FVector::DotProduct(Forward, Target->GetActorLocation().Normalize());
	//float AcosAngle = FMath::Acos(Dot);
	//float AngleDegree = FMath::RadiansToDegrees(AcosAngle);

	//Center에서 Target을 바라보는 Vector
	FVector ToTargetVec = (TargetLocation - GunMeshComp->GetSocketLocation(TEXT("Muzzle")));

	ToTargetVec.Z = 0;
	ToTargetVec.Normalize();

	//내적 후 아크 코사인을 통해 각을 구함(0~180)
	float InnerProduct = FVector::DotProduct(GetActorForwardVector(),ToTargetVec);
	float Degree = UKismetMathLibrary::DegAcos(InnerProduct);
    
	//외적으로 표면 벡터를 구해 Z의 값으로 부호를 구함
	FVector OutProduct = FVector::CrossProduct(GetActorForwardVector(),ToTargetVec);
	float Sign = UKismetMathLibrary::SignOfFloat(OutProduct.Z);

	return Degree*Sign;
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

