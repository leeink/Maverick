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
#include "XR_LSJ/AIUnitHpBar.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LDG/Soldier.h"
#include "LDG/SoldierAIController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HpBarNewIcon.h"
#include "LDG/TankBase.h"
#include "LDG/TankAIController.h"
#include "XR_LSJ/MinimapViewer.h"
#include "Sound/SoundBase.h"

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
	CurrentActionState= EAIUnitActionState::IDLE;
	AIUnitCategory = EAIUnitCategory::TANK;

	

	Tags.Add(TEXT("Enemy"));

	HpWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComp"));
    HpWidgetComp->SetupAttachment(BoxComp);
    HpWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HpWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    HpWidgetComp->SetDrawSize(FVector2D(100,100));
    HpWidgetComp->SetRelativeLocation(FVector(0,0.f,400.0f));

	MinimapHpWidgetSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MinimapHpWidgetSpringArm"));
	MinimapHpWidgetSpringArm->SetupAttachment(BoxComp);
	MinimapHpWidgetSpringArm->bInheritYaw=false;
	MinimapHpWidgetSpringArm->bInheritPitch=false;
	MinimapHpWidgetSpringArm->bInheritRoll=false;

	MinimapHpWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("MinimapHpWidgetComp"));
    MinimapHpWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MinimapHpWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    MinimapHpWidgetComp->SetDrawSize(FVector2D(100,100));
    MinimapHpWidgetComp->SetRelativeLocation(FVector(322.000000f,0.f,5000.000000f));
	MinimapHpWidgetComp->SetRelativeRotation(FRotator(90.000000,-90.000000,0.000000));
	MinimapHpWidgetComp->SetRelativeScale3D(FVector(1.000000,13.000000,13.000000));
	MinimapHpWidgetComp->bVisibleInSceneCaptureOnly=true;
	MinimapHpWidgetComp->SetCastShadow(false);
	MinimapHpWidgetComp->SetupAttachment(MinimapHpWidgetSpringArm);

	AIControllerClass = AAITankController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

EAIUnitCommandState AAITankPawn::GetCurrentCommandState()
{
	return CurrentCommandState;
}

EAIUnitActionState AAITankPawn::GetCurrentActionState()
{
	return CurrentActionState;
}

void AAITankPawn::SetMinimapUIZOrder(int32 Value)
{
    FVector Location = MinimapHpWidgetComp->GetRelativeLocation();
    Location.Z+=Value;
    MinimapHpWidgetComp->SetRelativeLocation(Location);
}
void AAITankPawn::SetCommandState(EAIUnitCommandState Command)
{
	CurrentCommandState = Command;
}

void AAITankPawn::SetActionState(EAIUnitActionState State)
{
	CurrentActionState=State;
	switch ( CurrentActionState )
	{
	case EAIUnitActionState::IDLE:
		GetController()->StopMovement();
		MoveWheelAnimation(0);
		break;
	case EAIUnitActionState::MOVE:
		MoveWheelAnimation(100);
		break;
	case EAIUnitActionState::IDLEATTACK:
		break;
	case EAIUnitActionState::MOVEATTACK:
		break;
	case EAIUnitActionState::DIE:
		MoveWheelAnimation(0);
		GetController()->StopMovement();
		if (FDelUnitDie.IsBound())
			FDelUnitDie.Execute();
		GetWorld()->GetTimerManager().ClearTimer(FindEnemy);
		if (FDelTankUnitDie.IsBound())
			FDelTankUnitDie.Execute();
		break;
	}
}

float AAITankPawn::GetLookTargetAngle(FVector TargetLocation)
{
	//Center에서 Target을 바라보는 Vector
	FVector ToTargetVec;
	if (Target != nullptr)
	{
		TargetLocation = Target->GetActorLocation();
		ToTargetVec = (TargetLocation - GetActorLocation());
	}
	else
	{
		return 0;
	}

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

void AAITankPawn::StopAttack()
{
	if(CurrentActionState==EAIUnitActionState::IDLEATTACK)
		SetActionState(EAIUnitActionState::IDLE);
	else 
		SetActionState(EAIUnitActionState::MOVE);
	FindCloseTargetUnit();
}

void AAITankPawn::AttackTargetUnit(AActor* TargetActor)
{
	Target = TargetActor;
	if(CurrentActionState==EAIUnitActionState::IDLE)
		SetActionState(EAIUnitActionState::IDLEATTACK);
	else 
		SetActionState(EAIUnitActionState::MOVEATTACK);
}

// Called when the game starts or when spawned
void AAITankPawn::BeginPlay()
{
	Super::BeginPlay();

	TankAbility.Hp = 10000.f;
	MaxTankHp = TankAbility.Hp;
	CurrentTankHp = MaxTankHp;
    TankAbility.FindTargetRange = 4000.f;
	TankAbility.ExplosiveRange = 600.f;
	TankAbility.ExplosiveMaxDamage = 105.f;
	TankAbility.ExplosiveMinDamage = 5.f;

	TurretRotSpeed = 50.0f;
	FireCoolTime = 3.5f;

	AITankController = Cast<AAITankController>(GetController());
	if (AITankController)
	{
		AITankController->FCallback_AIController_MoveCompleted.BindUFunction(this,FName("OnMoveCompleted"));
		//FindPath(FVector(1500,0,0));
		FindCloseTargetPlayerUnit();
	}
	//HpBar
    if (HpWidgetComp && HpBarClass)
    {
        HpWidgetComp->SetWidgetClass(HpBarClass);
        UAIUnitHpBar* HpBarUI =Cast<UAIUnitHpBar>(HpWidgetComp->GetUserWidgetObject());
		if (HpBarUI)
		    HpBarUI->SetUITankImage();
    }
	//MinimapHpBar
    if (MinimapHpWidgetComp&& MinimapHpWidgetClass)
    {
        MinimapHpWidgetComp->SetWidgetClass(MinimapHpWidgetClass);
        UHpBarNewIcon* HpBarNewIcon =Cast<UHpBarNewIcon>(MinimapHpWidgetComp->GetUserWidgetObject());
		if (HpBarNewIcon)
		{
			HpBarNewIcon->SetUITankImage();
			HpBarNewIcon->SetHpBar(1.0f);
		}
		    

    }

	MinimapViewer = Cast<AMinimapViewer>(UGameplayStatics::GetActorOfClass(GetWorld(),AMinimapViewer::StaticClass()));

	if(false==StartGoalLocation.Equals(FVector::ZeroVector))
		FindPath(StartGoalLocation);
	GetWorld()->GetTimerManager().SetTimer(FindEnemy, this, &AAITankPawn::FindCloseTargetPlayerUnit, 3.0f, true);

	SetVisibleTank(false);
}
void AAITankPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//델리게이트 해제
	FDelUnitDie.Unbind();
}
FVector AAITankPawn::GetTargetLocation()
{
	return GetActorLocation();
}
//가까운 적 탐색
void AAITankPawn::FindCloseTargetPlayerUnit()
{
    FVector Start = GetActorLocation();
    FVector End = GetActorLocation();
    float Radius = TankAbility.FindTargetRange;
    ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);
    bool bTraceComplex = false;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
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
			if(false ==HitResult.GetActor()->ActorHasTag("Player"))
                continue;
			    //적이 죽었다면
            ASoldier* TargetPlayerUnit = Cast<ASoldier>(HitResult.GetActor());
            if (TargetPlayerUnit)
            {
	            ASoldierAIController* controller = Cast<ASoldierAIController>(TargetPlayerUnit->GetController());
	            if (controller&&controller->GetCurrentState()==EState::Die)
	            {
		            //FSMComp->SetIsAttacking(false,nullptr);
		            continue;
	            }	
            }
			else if (ATankBase* TargetPlayerTankUnit = Cast<ATankBase>(HitResult.GetActor()))
			{
				ATankAIController* controller = Cast<ATankAIController>(TargetPlayerTankUnit->GetController());
	            if (controller&&controller->CurrentState==ETankState::Die)
	            {
		            //FSMComp->SetIsAttacking(false,nullptr);
		            continue;
	            }
				else if (controller == nullptr)
				{
                    //UE_LOG(LogTemp,Error,TEXT("controller Die"));
					continue;
				}
			}
            
            //찾은 적들 중 가장 가까운 적이고 중간에 장애물이 없다면 타겟으로 지정
            //타겟에게 공격 지시
			FHitResult OutHit;
			FVector StartLocation = MeshComp->GetSocketLocation(TEXT("gun_jntSocket"));
			FVector EndLocation = HitResult.GetActor()->GetActorLocation();

			ECollisionChannel TraceChannelHit = ECC_GameTraceChannel5;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			Params.AddIgnoredActor(HitResult.GetActor());
			bool CanHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannelHit, Params);
			if (CanHit)
			{
				//DrawDebugLine(GetWorld(), StartLocation, HitResult.GetActor()->GetActorLocation() , FColor::Blue,false,10.0f);
			}
			else
			{
				
				AttackTargetUnit(HitResult.GetActor());
				return;
				//DrawDebugLine(GetWorld(), StartLocation, HitResult.GetActor()->GetActorLocation(), FColor::Red, false, 10.0f);
			}
        }
    }
    else //탐색 범위 안에서 적을 찾을 수 없다면
    {
		Target = nullptr;
		if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
			SetActionState(EAIUnitActionState::MOVE);
		else
			SetActionState(EAIUnitActionState::IDLE);
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
			FVector EndLocation = FoundEnemy->GetTargetLocation();

			ECollisionChannel TraceChannelHit = ECC_Visibility;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			Params.AddIgnoredActor(HitResult.GetActor());
			bool CanHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannelHit, Params);
			if (CanHit)
			{
				//DrawDebugLine(GetWorld(), StartLocation, HitResult.GetActor()->GetActorLocation() , FColor::Blue,false,10.0f);
			}
			else
			{
				FoundEnemy->FDelUnitDie.Unbind();
				FoundEnemy->FDelUnitDie.BindUFunction(this,FName("StopAttack"));
				AttackTargetUnit(HitResult.GetActor());
				//DrawDebugLine(GetWorld(), StartLocation, HitResult.GetActor()->GetActorLocation(), FColor::Red, false, 10.0f);
				return;
			}
        }
    }
    else //탐색 범위 안에서 적을 찾을 수 없다면
    {
		Target = nullptr;
		if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
			SetActionState(EAIUnitActionState::MOVE);
		else
			SetActionState(EAIUnitActionState::IDLE);
    }
}

void AAITankPawn::AddViewCount()
{
	viewCount++;
	if (IsHidden())
	{
		SetVisibleTank(true);
	}
}
void AAITankPawn::MinusViewCount()
{
	viewCount--;
	if (viewCount <= 0)
	{
		SetVisibleTank(false);
	}
}

void AAITankPawn::SetViewCountToHidden()
{
	viewCount=0;
	SetVisibleTank(false);
}

void AAITankPawn::SetVisibleTank(bool val)
{
	MinimapHpWidgetComp->SetVisibility(val);
	HpWidgetComp->SetVisibility(val);
	SetActorHiddenInGame(!val);
}
void AAITankPawn::FindPath(const FVector& TargetLocation)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (nullptr == NavSystem)
		return;

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
		UE_LOG(LogTemp, Warning, TEXT("Failed final destination 1!"));
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
			if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
			SetActionState(EAIUnitActionState::IDLEATTACK);
		else 
			SetCommandState(EAIUnitCommandState::IDLE);
			UE_LOG(LogTemp, Warning, TEXT("Failed final destination 2!"));
		}
	}
	else
	{
		if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
			SetActionState(EAIUnitActionState::IDLEATTACK);
		else 
			SetCommandState(EAIUnitCommandState::IDLE);
		UE_LOG(LogTemp, Warning, TEXT("Failed final destination 3!"));
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
		if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
			SetActionState(EAIUnitActionState::IDLEATTACK);
		else 
			SetCommandState(EAIUnitCommandState::IDLE);
		UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
	}
}
bool AAITankPawn::CalculateBallisticVelocity(
    const FVector& StartLocation, 
    const FVector& EndLocation, 
    float DesiredTime, // 목표 도달 시간
    FVector& OutVelocity)
{
    FVector Delta = EndLocation - StartLocation;
    float DistXZ = FVector(Delta.X, Delta.Y, 0.f).Size();
    float DistY = Delta.Z;
    float Gravity = FMath::Abs(GetWorld()->GetGravityZ()); // 중력 가속도
    
    // 필요한 초기 속도를 계산
    float InitialVelocityZ = (DistY + 0.5f * Gravity * DesiredTime * DesiredTime) / DesiredTime;
    float InitialVelocityXZ = DistXZ / DesiredTime;

    // 속도 벡터 생성
    FVector Direction = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();
    OutVelocity = Direction * InitialVelocityXZ;
    OutVelocity.Z = InitialVelocityZ;

    return true;
}

void AAITankPawn::FireCannon()
{
	if (nullptr == Target)
	{
		return;
	}
		
	 ASoldier* TargetPlayerUnit = Cast<ASoldier>(Target);
    if (TargetPlayerUnit)
    {
	    ASoldierAIController* controller = Cast<ASoldierAIController>(TargetPlayerUnit->GetController());
	    if (controller&&(controller->GetCurrentState()==EState::Die||controller->IsDead()))
	    {      
			if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
				SetActionState(EAIUnitActionState::MOVE);
			else
				SetActionState(EAIUnitActionState::IDLE);
			Target = nullptr;
			FindCloseTargetPlayerUnit();
		    return;
	    }
		else if (controller==nullptr)
		{
			if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
				SetActionState(EAIUnitActionState::MOVE);
			else
				SetActionState(EAIUnitActionState::IDLE);
			Target = nullptr;
			FindCloseTargetPlayerUnit();
		    return;
		}
    }
	ATankBase* TargetPlayerTankUnit = Cast<ATankBase>(Target);
	if (TargetPlayerTankUnit)
	{
		ATankAIController* controller = Cast<ATankAIController>(TargetPlayerTankUnit->GetController());
		if (controller && (controller->CurrentState == ETankState::Die))
		{
			if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
				SetActionState(EAIUnitActionState::MOVE);
			else
				SetActionState(EAIUnitActionState::IDLE);
			Target = nullptr;
			FindCloseTargetPlayerUnit();
		    return;
		}
		else if (controller==nullptr)
		{
			if(CurrentActionState == EAIUnitActionState::MOVEATTACK)
				SetActionState(EAIUnitActionState::MOVE);
			else
				SetActionState(EAIUnitActionState::IDLE);
			Target = nullptr;
			FindCloseTargetPlayerUnit();
		    return;
		}
	}

	AddViewCount();
	FTimerHandle VisibleHandle;
	GetWorld()->GetTimerManager().SetTimer(VisibleHandle,[&]()
	{
		MinusViewCount();
	},2.0f,false);

	//목표에 도달하기 위해 총알 Velocity 구하기
	FVector OutVelocity;
	float ArrivalTime = FVector::Distance(Target->GetActorLocation() , MeshComp->GetSocketLocation(TEXT("gun_jntSocket")))*(.0002f);
	FVector TargetLocation = Target->GetActorLocation();
	TargetLocation.Z /= 2;
	//UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVelocity, MeshComp->GetSocketLocation(TEXT("gun_jntSocket")), TargetLocation,0,0.5f);
		//GetWorld()->GetGravityZ(),.9f);

	CalculateBallisticVelocity(MeshComp->GetSocketLocation(TEXT("gun_jntSocket")), TargetLocation,ArrivalTime,OutVelocity);

	
	// 총 사운드
	if(GunFireSound&&SA_Unit)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),GunFireSound,GetActorLocation(),.8f,1.0f,0.f,SA_Unit);
	
	//muzzle 이펙트 
	FireFx(true);
	//총알 방향
	//FVector LaunchDirection = (Target->GetActorLocation() - MeshComp->GetSocketLocation(TEXT("gun_jntSocket"))).GetSafeNormal();
	//총알 소환
	AAITankBullet* Bullet = GetWorld()->SpawnActor<AAITankBullet>(BulletFactory,MeshComp->GetSocketLocation(TEXT("gun_jntSocket")),MeshComp->GetSocketRotation(TEXT("gun_jntSocket")));
	if (Bullet)
	{
		Bullet->SetOwner(this);
		Bullet->SetExplosiveRange(TankAbility.ExplosiveRange);
		Bullet->SetExplosiveMaxDamage(TankAbility.ExplosiveMaxDamage);
		Bullet->SetExplosiveMinDamage(TankAbility.ExplosiveMinDamage);
		Bullet->InitMovement(OutVelocity);
	}
	//미니맵에 경고 UI 표시
	CreateWarningUIToMinimap();
}
void AAITankPawn::CreateWarningUIToMinimap()
{
	if(MinimapViewer)
		MinimapViewer->CreateWarningUI(GetActorLocation());
}
void AAITankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(CurrentCommandState == EAIUnitCommandState::DIE)
		return;
	if (GetCurrentActionState() == EAIUnitActionState::IDLEATTACK || GetCurrentActionState() == EAIUnitActionState::MOVEATTACK)
	{
		FireTotalTime+=DeltaTime;
		if ( GetLookTargetAngle(FVector::ZeroVector) - TurretRotation > 1.f)
		{
			TurretRotation += DeltaTime * TurretRotSpeed;
			RotateYawurret(TurretRotation);
		}
		else if ( GetLookTargetAngle(FVector::ZeroVector) - TurretRotation < -1.f)
		{
			TurretRotation -= DeltaTime * TurretRotSpeed;
			RotateYawurret(TurretRotation);
		}
		else
		{ 
			if (FireTotalTime >= FireCoolTime)
			{	
				FireTotalTime=0;
				FireCannon();
			}
		}
	}
	else
	{
		if (GetLookTargetAngle(FVector::ZeroVector) - TurretRotation > 1.f)
		{
			TurretRotation += DeltaTime * TurretRotSpeed;
			RotateYawurret(TurretRotation);
		}
		else if (GetLookTargetAngle(FVector::ZeroVector) -TurretRotation < -1.f)
		{
			TurretRotation -= DeltaTime * TurretRotSpeed;
			RotateYawurret(TurretRotation);
		}
	}
}

// Called to bind functionality to input
void AAITankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAITankPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);

	if (Damage > 0)
	{
		CurrentTankHp -= Damage;
		UAIUnitHpBar* HpBarUI =Cast<UAIUnitHpBar>(HpWidgetComp->GetUserWidgetObject());
		if(nullptr==HpBarUI)
			return Damage;
		UHpBarNewIcon* HpBarNewIcon =Cast<UHpBarNewIcon>(MinimapHpWidgetComp->GetUserWidgetObject());
		if(nullptr==HpBarNewIcon)
			return Damage;
		CurrentTankHp -= Damage;
		HpBarUI->SetHpBar((float)(CurrentTankHp) / MaxTankHp);
		HpBarNewIcon->SetHpBar((float)(CurrentTankHp) / MaxTankHp);
		if (CurrentTankHp <= 0)
		{
			SetCommandState(EAIUnitCommandState::DIE);
			SetActionState(EAIUnitActionState::DIE);
			HpBarUI->SetVisibility(ESlateVisibility::Collapsed);
			HpWidgetComp->Deactivate();
			HpBarNewIcon->SetVisibility(ESlateVisibility::Collapsed);
			MinimapHpWidgetComp->Deactivate();
			DieAnimation(true);
		}
	}
	return Damage;
}