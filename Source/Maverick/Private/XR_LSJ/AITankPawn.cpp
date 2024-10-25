// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AITankPawn.h"
#include "XR_LSJ/AITankController.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "XR_LSJ/AISquad.h"
#include "NavigationSystem.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AI/Navigation/NavigationTypes.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "XR_LSJ/AITankBullet.h"

// Sets default values
AAITankPawn::AAITankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetRelativeLocation(FVector(0,0,-100.f));
	BoxComp->SetBoxExtent(FVector(350.f,180.f,100.f));
	SetRootComponent(BoxComp);
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComp);
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	CurrentCommandState = EAIUnitCommandState::IDLE;
	AIUnitCategory = EAIUnitCategory::TANK;

	TankAbility.Hp = 100.f;
    MaxHp = TankAbility.Hp;
    CurrentHp = MaxHp;
    TankAbility.FindTargetRange = 2000.f;
	TurretRotSpeed = 50.0f;
	FireCoolTime = 2.0f;
	Tags.Add(TEXT("Enemy"));

	AIControllerClass = AAITankController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

EAIUnitCommandState AAITankPawn::GetCurrentCommandState()
{
	return CurrentCommandState;
}

void AAITankPawn::SetCommandState(EAIUnitCommandState Command)
{

	if (Command != EAIUnitCommandState::MOVE)
	{
		GetController()->StopMovement();
	}
	CurrentCommandState = Command;
	switch ( CurrentCommandState )
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
float AAITankPawn::GetLookTargetAngle(FVector TargetLocation)
{
	if(Target == nullptr)
		return 0.0f;
	TargetLocation = Target->GetActorLocation();
	//Center에서 Target을 바라보는 Vector
	FVector ToTargetVec = (TargetLocation - GetActorLocation());

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
void AAITankPawn::AttackTargetUnit(AActor* TargetActor)
{
	Target = TargetActor;
	SetCommandState(EAIUnitCommandState::ATTACK);
	
	//타겟 바라보기
	
}

// Called when the game starts or when spawned
void AAITankPawn::BeginPlay()
{
	Super::BeginPlay();
	AITankController = Cast<AAITankController>(GetController());
	if (AITankController)
	{
		AITankController->FCallback_AIController_MoveCompleted.BindUFunction(this,FName("OnMoveCompleted"));
		FindPath(FVector(1025.958464,1622.088644,118.775006));
		FindCloseTargetUnit();
	}
	
}

//가까운 적 탐색
void AAITankPawn::FindCloseTargetUnit()
{
    FVector Start = GetActorLocation();
    FVector End = GetActorLocation();
    float Radius = TankAbility.FindTargetRange;
    ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);
    bool bTraceComplex = false;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::ForOneFrame;
    TArray<FHitResult> OutHits;
    bool bIgnoreSelf = true;
    FLinearColor TraceColor = FLinearColor::Gray;
    FLinearColor TraceHitColor = FLinearColor::Blue;
    float DrawTime = 1.0f;
    //SquadManagerAbility.FindTargetRange 범위 탐색
    const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,Radius,TraceChannel,bTraceComplex,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf,TraceColor,TraceHitColor);
    if (Hit)
    {
        for (const FHitResult& HitResult : OutHits)
        {
            IIAICommand* FoundEnemy = Cast<IIAICommand>(HitResult.GetActor());
            if(nullptr == FoundEnemy)
                continue;
            if(FoundEnemy && FoundEnemy->GetCurrentCommandState()==EAIUnitCommandState::DIE)
                continue;
            if(false ==HitResult.GetActor()->ActorHasTag("Enemy"))
                continue;
            
            //찾은 적들 중 가장 가까운 적이고 중간에 장애물이 없다면 타겟으로 지정
            //타겟에게 공격 지시
			FHitResult OutHit;
			FVector StartLocation = MeshComp->GetSocketLocation(TEXT("gun_jntSocket"));
			FVector EndLocation;
			if (FoundEnemy->GetAIUnitCategory() == EAIUnitCategory::SQUAD)
			{
				AAISquad* AISquad = Cast<AAISquad>(HitResult.GetActor());
				AISquad->GetMesh()->GetSocketLocation(TEXT("Head"));
			}
			else
				EndLocation = HitResult.GetActor()->GetActorLocation();

			ECollisionChannel TraceChannelHit = ECC_Visibility;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			Params.AddIgnoredActor(HitResult.GetActor());
			bool CanHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannelHit, Params);
			if (CanHit)
			{
				DrawDebugLine(GetWorld(), StartLocation, HitResult.GetActor()->GetActorLocation() , FColor::Blue,false,10.0f);
			}
			else
			{
				AttackTargetUnit(HitResult.GetActor());
				DrawDebugLine(GetWorld(), StartLocation, HitResult.GetActor()->GetActorLocation(), FColor::Red, false, 10.0f);
				return;
			}
        }
    }
    else //탐색 범위 안에서 적을 찾을 수 없다면
    {

    }
}

void AAITankPawn::FindPath(const FVector& TargetLocation)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (nullptr == NavSystem)
		return;

	// 지원되는 에이전트 설정을 가져오기
    //const TArray<FNavDataConfig>& SupportedAgents = NavSystem->SupportedAgen();

 

	FVector StartLocation =  GetActorLocation();
	UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), StartLocation, TargetLocation);

	if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
        TArray<FVector> ArrayLocation;
        ArrayLocation = NavPath->PathPoints;
		MovePathAsync(ArrayLocation);
    }
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // 경로가 끊겼을때
    {
         
    }
}
void AAITankPawn::OnMoveCompleted(EPathFollowingResult::Type Result)
{
    // 이동이 성공적으로 완료된 경우에만 다음 지점으로 이동
    if (Result == EPathFollowingResult::Success)
    {
        CurrentPathPointIndex++;
		 
		 if (CurrentPathPointIndex < (PathVectorArray.Num()))
		{
			//// 다음 경로 지점으로 이동
			 FVector NextPoint = PathVectorArray[CurrentPathPointIndex];
			 AITankController->MoveToLocation(NextPoint, 100.0f);
		}
		else
		{
			SetCommandState(EAIUnitCommandState::IDLE);
			//UE_LOG(LogTemp, Warning, TEXT("Reached final destination 3! %d %s"),(int)Result,*AISquadBody->GetName());
		}
	}
	else
	{
		SetCommandState(EAIUnitCommandState::IDLE);
		//UE_LOG(LogTemp, Warning, TEXT("Failed final destination 3! %d %s"),(int)Result,*AISquadBody->GetName());
	}
}
void AAITankPawn::MovePathAsync(TArray<FVector>& NavPathArray)
{
	SetCommandState(EAIUnitCommandState::MOVE);
	PathVectorArray = NavPathArray;
	CurrentPathPointIndex = 1;
	AITankController->FCallback_AIController_MoveCompleted.Unbind();

	//남은 경로 지점이 있는지 확인
		if (CurrentPathPointIndex < (PathVectorArray.Num()))
		{
			//다음 경로 지점으로 이동
			FVector NextPoint = PathVectorArray[CurrentPathPointIndex];
			AITankController->MoveToLocation(NextPoint, 100.0f);
			
			//이동 완료 후 다시 OnMoveCompleted 호출
			AITankController->FCallback_AIController_MoveCompleted.BindUObject(this, &AAITankPawn::OnMoveCompleted);
		}
		else
		{
			SetCommandState(EAIUnitCommandState::IDLE);
			UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
		}
}
void AAITankPawn::FireCannon()
{
	
	//muzzle 이펙트 
	FireFx(true);
	//총알 방향
	FVector LaunchDirection = (Target->GetActorLocation() - MeshComp->GetSocketLocation(TEXT("gun_jntSocket"))).GetSafeNormal();
	//총알 소환
	AAITankBullet* Bullet = GetWorld()->SpawnActor<AAITankBullet>(BulletFactory,MeshComp->GetSocketLocation(TEXT("gun_jntSocket")),MeshComp->GetSocketRotation(TEXT("gun_jntSocket")));
	if (Bullet)
	{
		Bullet->SetOwner(this);
		//FVector BulletScale = FVector(100,100,100);
		Bullet->InitMovement(LaunchDirection);
	}
}
// Called every frame
void AAITankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCurrentCommandState() == EAIUnitCommandState::ATTACK)
	{
		if ( GetLookTargetAngle(FVector::ZeroVector) - TurretRotation > 1.f)
		{
			TurretRotation += DeltaTime * TurretRotSpeed;
		}
		else if ( GetLookTargetAngle(FVector::ZeroVector) - TurretRotation < -1.f)
		{
			TurretRotation -= DeltaTime * TurretRotSpeed;
		}
		else
		{
			FireTotalTime+=DeltaTime;
			if (FireTotalTime >= FireCoolTime)
			{	
				FireTotalTime=0;
				FireCannon();
			}
			
		}
		RotateYawurret(TurretRotation);
	}
		
}

// Called to bind functionality to input
void AAITankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

