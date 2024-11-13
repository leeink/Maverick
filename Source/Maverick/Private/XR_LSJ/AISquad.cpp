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
#include "LDG/Soldier.h"
#include "LDG/SoldierAIController.h"

// Sets default values
AAISquad::AAISquad()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FSMComp = CreateDefaultSubobject<UAISquadFSMComponent>(TEXT("FSMComp"));
	
	SetGunMeshComp(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp")));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshFinder(TEXT("/Game/Asset/FPS_Weapon_Bundle/Weapons/Meshes/Ka47/SK_KA47.SK_KA47"));
	if (GunMeshFinder.Succeeded())
	{
		GetGunMeshComp()->SetSkeletalMesh(GunMeshFinder.Object);
	}
	GetGunMeshComp()->SetupAttachment(GetMesh(),FName("RightHandThumb4"));
	GetGunMeshComp()->SetRelativeLocation(FVector(-3.612750,1.138105,3.562393));
	GetGunMeshComp()->SetRelativeRotation(FRotator(23.987173,20.151969,-190.523942));
	
	Tags.Add("Enemy");

	AIControllerClass = AAISquadController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

FVector AAISquad::GetTargetLocation()
{
	return GetMesh()->GetSocketLocation(TEXT("Head"));
}

float AAISquad::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
	if(SquadAbility.Hp<=0)
		return Damage;
	//GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Red,TEXT("Damage"));
	if (Damage > 0)
	{
		int32 BeforeHp = SquadAbility.Hp;
		SquadAbility.Hp=FMath::Max(SquadAbility.Hp - Damage,0);
		int32 HpLost = BeforeHp-SquadAbility.Hp;
		
		if (FDelSquadUnitDamaged.IsBound())
			FDelSquadUnitDamaged.Execute(HpLost);

		if (SquadAbility.Hp <= 0)
		{
			FSMComp->SetState(EEnemyState::DIE);
			GetController()->StopMovement();
			if(FDelSquadUnitDie.IsBound())
				FDelSquadUnitDie.Execute(MySquadNumber);
			//UE_LOG(LogTemp,Error,TEXT("MySquadNumber %d %s"),MySquadNumber,*GetName());
			if(FDelUnitDie.IsBound())
				FDelUnitDie.Execute();
		}


	}
	return Damage;
}

void AAISquad::AttackFire()
{
	//적이 죽었다면
	ASoldier* TargetPlayerUnit = Cast<ASoldier>(FSMComp->GetTarget());
	if (TargetPlayerUnit)
	{
		ASoldierAIController* controller = Cast<ASoldierAIController>(TargetPlayerUnit->GetController());
		if (controller&&(controller->IsDead()||controller->GetCurrentState()==EState::Die))
		{
			FSMComp->SetTarget(nullptr);
			if(FDelTargetDie.IsBound())
				FDelTargetDie.Execute();
			return;
		}
		else if (controller == nullptr)
		{
			FSMComp->SetTarget(nullptr);
			if(FDelTargetDie.IsBound())
				FDelTargetDie.Execute();
			return;
		}
	}
	IIAICommand* TargetUnit = Cast<IIAICommand>(FSMComp->GetTarget());
	if (TargetUnit && TargetUnit->GetCurrentCommandState() == EAIUnitCommandState::DIE || nullptr == FSMComp->GetTarget())
	{
		if(FDelTargetDie.IsBound())
			FDelTargetDie.Execute();
		return;
	}
	//muzzle 이펙트 
	GunMuzzleFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(GunMuzzleFXSystem,GetGunMeshComp(),TEXT("Muzzle"),FVector::ZeroVector,FRotator::ZeroRotator,FVector(5,5,5),EAttachLocation::SnapToTarget,true,ENCPoolMethod::AutoRelease);
	
	//총알 방향
	FVector LaunchDirection = (FSMComp->GetTarget()->GetActorLocation() - GetGunMeshComp()->GetSocketLocation(TEXT("Muzzle"))).GetSafeNormal();
	//총알 소환
	AAISquadBullet* Bullet = GetWorld()->SpawnActor<AAISquadBullet>(BulletFactory,GetGunMeshComp()->GetSocketLocation(TEXT("Muzzle")),GetGunMeshComp()->GetSocketRotation(TEXT("Muzzle")));
	if (Bullet)
	{
		Bullet->SetOwner(this);
		Bullet->InitMovement(LaunchDirection);
	}
}
EAIUnitCommandState AAISquad::GetCurrentCommandState()
{
	return CurrentCommandState;
}
void AAISquad::SetCommandState(EAIUnitCommandState Command)
{
	CurrentCommandState = Command;
	switch (CurrentCommandState)
	{
	case EAIUnitCommandState::IDLE:
		break;
	case EAIUnitCommandState::MOVE:
		break;
	case EAIUnitCommandState::ATTACK:
		break;
	case EAIUnitCommandState::DAMAGE:
		break;
	case EAIUnitCommandState::DIE:
		
		break;
	default:
		break;
	}
}
void AAISquad::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//델리게이트 해제
	FDelUnitDie.Unbind();
	FDelTargetDie.Unbind();
	FDelSquadUnitDie.Unbind();
	FDelFailToDestination.Unbind();
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
float AAISquad::GetLookTargetAngle(FVector TargetLocation)
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
	FVector ToTargetVec = (TargetLocation - GetGunMeshComp()->GetSocketLocation(TEXT("Muzzle")));

	ToTargetVec.Z = 0;
	ToTargetVec.Normalize();

	//내적 후 아크 코사인을 통해 각을 구함(0~180)
	float InnerProduct = FVector::DotProduct(GetActorForwardVector(),ToTargetVec);
	float Degree = UKismetMathLibrary::DegAcos(InnerProduct) + 2.5f;
    
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

