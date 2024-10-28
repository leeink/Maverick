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

	

	Tags.Add(TEXT("Enemy"));

	HpWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComp"));
    HpWidgetComp->SetupAttachment(BoxComp);
    HpWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HpWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    HpWidgetComp->SetDrawSize(FVector2D(100,100));
    HpWidgetComp->SetRelativeLocation(FVector(0,0.f,400.0f));

	AIControllerClass = AAITankController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

EAIUnitCommandState AAITankPawn::GetCurrentCommandState()
{
	return CurrentCommandState;
}

void AAITankPawn::SetCommandState(EAIUnitCommandState Command)
{
	if(nullptr!=Target && Command == EAIUnitCommandState::IDLE)
		return;
	if (CurrentCommandState != EAIUnitCommandState::ATTACK)
		PreState = CurrentCommandState;

	CurrentCommandState = Command;

	UE_LOG(LogTemp, Warning, TEXT("PreState %d"),(int)PreState);
	UE_LOG(LogTemp, Warning, TEXT("CurrentCommandState %d"),(int)CurrentCommandState);

	switch ( CurrentCommandState )
	{
	case EAIUnitCommandState::IDLE:

		GetController()->StopMovement();
		MoveWheelAnimation(0);
		break;
	case EAIUnitCommandState::MOVE:
		MoveWheelAnimation(MovementComponent->GetMaxSpeed());
		break;
	case EAIUnitCommandState::ATTACK:
		
		break;
	case EAIUnitCommandState::DAMAGE:
		
		break;
	case EAIUnitCommandState::DIE:
		if(FDelUnitDie.IsBound())
			FDelUnitDie.Execute();
		DieAnimation(true);

		break;
	default:
		
		break;
	}
}
float AAITankPawn::GetLookTargetAngle(FVector TargetLocation)
{
	//Center���� Target�� �ٶ󺸴� Vector
	FVector ToTargetVec;
	if (Target != nullptr)
	{
		TargetLocation = Target->GetActorLocation();
		ToTargetVec = (TargetLocation - GetActorLocation());
	}
	else
	{
		TargetLocation = MeshComp->GetForwardVector()* -1000.f;
		ToTargetVec = (GetActorLocation() - TargetLocation);
	}

	ToTargetVec.Z = 0;
	ToTargetVec.Normalize();

	//���� �� ��ũ �ڻ����� ���� ���� ����(0~180)
	float InnerProduct = FVector::DotProduct(GetActorForwardVector(),ToTargetVec);
	float Degree = UKismetMathLibrary::DegAcos(InnerProduct) + 2.5f;
    
	//�������� ǥ�� ���͸� ���� Z�� ������ ��ȣ�� ����
	FVector OutProduct = FVector::CrossProduct(GetActorForwardVector(),ToTargetVec);
	float Sign = UKismetMathLibrary::SignOfFloat(OutProduct.Z);

	return Degree*Sign;
}

void AAITankPawn::StopAttack()
{
	SetCommandState(EAIUnitCommandState::IDLE);
	FindCloseTargetUnit();
}

void AAITankPawn::AttackTargetUnit(AActor* TargetActor)
{
	Target = TargetActor;
	SetCommandState(EAIUnitCommandState::ATTACK);
	
	//Ÿ�� �ٶ󺸱�
	
}

// Called when the game starts or when spawned
void AAITankPawn::BeginPlay()
{
	Super::BeginPlay();

	TankAbility.Hp = 100.f;
	MaxTankHp = TankAbility.Hp;
	CurrentTankHp = MaxTankHp;
    TankAbility.FindTargetRange = 10000.f;
	TankAbility.ExplosiveRange = 600.f;
	TankAbility.ExplosiveMaxDamage = 105.f;
	TankAbility.ExplosiveMinDamage = 5.f;

	TurretRotSpeed = 50.0f;
	FireCoolTime = 2.0f;

	AITankController = Cast<AAITankController>(GetController());
	if (AITankController)
	{
		AITankController->FCallback_AIController_MoveCompleted.BindUFunction(this,FName("OnMoveCompleted"));
		FindPath(FVector(1025.958464,1622.088644,118.775006));
		FindCloseTargetUnit();
	}
	//HpBar
    if (HpWidgetComp && HpBarClass)
    {
        HpWidgetComp->SetWidgetClass(HpBarClass);
        UAIUnitHpBar* HpBarUI =Cast<UAIUnitHpBar>(HpWidgetComp->GetUserWidgetObject());
		if (HpBarUI)
		    HpBarUI->SetUITankImage();
    }
}
void AAITankPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//��������Ʈ ����
	FDelUnitDie.Unbind();
}
FVector AAITankPawn::GetTargetLocation()
{
	return GetActorLocation();
}

//����� �� Ž��
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
    //SquadManagerAbility.FindTargetRange ���� Ž��
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
            
            //ã�� ���� �� ���� ����� ���̰� �߰��� ��ֹ��� ���ٸ� Ÿ������ ����
            //Ÿ�ٿ��� ���� ����
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
    else //Ž�� ���� �ȿ��� ���� ã�� �� ���ٸ�
    {
		Target = nullptr;
		SetCommandState(PreState);
    }
}

void AAITankPawn::FindPath(const FVector& TargetLocation)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (nullptr == NavSystem)
		return;

	// �����Ǵ� ������Ʈ ������ ��������
    //const TArray<FNavDataConfig>& SupportedAgents = NavSystem->SupportedAgen();

 

	FVector StartLocation =  GetActorLocation();
	UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), StartLocation, TargetLocation);

	if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
        TArray<FVector> ArrayLocation;
        ArrayLocation = NavPath->PathPoints;
		MovePathAsync(ArrayLocation);
    }
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // ��ΰ� ��������
    {
         
    }
}
void AAITankPawn::OnMoveCompleted(EPathFollowingResult::Type Result)
{
    // �̵��� ���������� �Ϸ�� ��쿡�� ���� �������� �̵�
    if (Result == EPathFollowingResult::Success)
    {
        CurrentPathPointIndex++;
		 
		 if (CurrentPathPointIndex < (PathVectorArray.Num()))
		{
			//// ���� ��� �������� �̵�
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

	//���� ��� ������ �ִ��� Ȯ��
		if (CurrentPathPointIndex < (PathVectorArray.Num()))
		{
			//���� ��� �������� �̵�
			FVector NextPoint = PathVectorArray[CurrentPathPointIndex];
			AITankController->MoveToLocation(NextPoint, 100.0f);
			
			//�̵� �Ϸ� �� �ٽ� OnMoveCompleted ȣ��
			AITankController->FCallback_AIController_MoveCompleted.BindUObject(this, &AAITankPawn::OnMoveCompleted);
		}
		else
		{
			SetCommandState(EAIUnitCommandState::IDLE);
			UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
		}
}
bool AAITankPawn::CalculateBallisticVelocity(
    const FVector& StartLocation, 
    const FVector& EndLocation, 
    float DesiredTime, // ��ǥ ���� �ð�
    FVector& OutVelocity)
{
    FVector Delta = EndLocation - StartLocation;
    float DistXZ = FVector(Delta.X, Delta.Y, 0.f).Size();
    float DistY = Delta.Z;
    float Gravity = FMath::Abs(GetWorld()->GetGravityZ()); // �߷� ���ӵ�
    
    // �ʿ��� �ʱ� �ӵ��� ���
    float InitialVelocityZ = (DistY + 0.5f * Gravity * DesiredTime * DesiredTime) / DesiredTime;
    float InitialVelocityXZ = DistXZ / DesiredTime;

    // �ӵ� ���� ����
    FVector Direction = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();
    OutVelocity = Direction * InitialVelocityXZ;
    OutVelocity.Z = InitialVelocityZ;

    return true;
}

void AAITankPawn::FireCannon()
{
	if(nullptr == Target)
		return;
	//��ǥ�� �����ϱ� ���� �Ѿ� Velocity ���ϱ�
	FVector OutVelocity;
	float ArrivalTime = FVector::Distance(Target->GetActorLocation() , MeshComp->GetSocketLocation(TEXT("gun_jntSocket")))*(.0002f);
	FVector TargetLocation = Target->GetActorLocation();
	TargetLocation.Z /= 2;
	//UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVelocity, MeshComp->GetSocketLocation(TEXT("gun_jntSocket")), TargetLocation,0,0.5f);
		//GetWorld()->GetGravityZ(),.9f);

	CalculateBallisticVelocity(MeshComp->GetSocketLocation(TEXT("gun_jntSocket")), TargetLocation,ArrivalTime,OutVelocity);

	
	//float LaunchSpeed = 13300.f; // �ʱ� �߻� �ӵ� ����
	
	//muzzle ����Ʈ 
	FireFx(true);
	//�Ѿ� ����
	//FVector LaunchDirection = (Target->GetActorLocation() - MeshComp->GetSocketLocation(TEXT("gun_jntSocket"))).GetSafeNormal();
	//�Ѿ� ��ȯ
	AAITankBullet* Bullet = GetWorld()->SpawnActor<AAITankBullet>(BulletFactory,MeshComp->GetSocketLocation(TEXT("gun_jntSocket")),MeshComp->GetSocketRotation(TEXT("gun_jntSocket")));
	if (Bullet)
	{
		Bullet->SetOwner(this);
		//FVector BulletScale = FVector(100,100,100);
		
		//UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVelocity, MeshComp->GetSocketLocation(TEXT("gun_jntSocket")), Target->GetActorLocation(),
		//GetWorld()->GetGravityZ(),.9f);
		Bullet->SetExplosiveRange(TankAbility.ExplosiveRange);
		Bullet->SetExplosiveMaxDamage(TankAbility.ExplosiveMaxDamage);
		Bullet->SetExplosiveMinDamage(TankAbility.ExplosiveMinDamage);
		Bullet->InitMovement(OutVelocity);
	}
}
// Called every frame
void AAITankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCurrentCommandState() == EAIUnitCommandState::ATTACK)
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
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Red,TEXT("Damage"));
	if (Damage > 0)
	{
		CurrentTankHp -= Damage;
		UAIUnitHpBar* HpBarUI =Cast<UAIUnitHpBar>(HpWidgetComp->GetUserWidgetObject());
		if(nullptr==HpBarUI)
			return Damage;
		HpBarUI->SetHpBar((float)(CurrentTankHp -= Damage) / MaxTankHp);
		if (CurrentTankHp <= 0)
		{
			SetCommandState(EAIUnitCommandState::DIE);
			HpBarUI->SetVisibility(ESlateVisibility::Collapsed);
			HpWidgetComp->Deactivate();
		}
	}
	return Damage;
}