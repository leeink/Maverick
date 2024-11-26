// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadFSMComponent.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "XR_LSJ/AISquad.h"
#include "CollisionQueryParams.h"
#include "NavigationPath.h"
#include "XR_LSJ/AISquadController.h"
#include "XR_LSJ/AISquadAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UAISquadFSMComponent::UAISquadFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

void UAISquadFSMComponent::SetState(EEnemyState NextState)
{
	EEnemyState prevState = GetCurrentState();
	SetCurrentState(NextState);

	// 애니메이션의 상태도 동기화 하고싶다.
	//Anim->EnemyState = NextState;

	//CurrentTime = 0;
	if (NextState != EEnemyState::MOVE && NextState != EEnemyState::ATTACK)
	{
		AISquadController->StopMovement();
	}
	
	switch ( GetCurrentState() )
	{
	case EEnemyState::IDLE:
		AISquadBody->SetCommandState(EAIUnitCommandState::IDLE);
		break;
	case EEnemyState::MOVE:
		AISquadBody->SetCommandState(EAIUnitCommandState::MOVE);

		break;
	case EEnemyState::ATTACK:
		AISquadBody->SetCommandState(EAIUnitCommandState::ATTACK);
		break;
	case EEnemyState::DAMAGE:
		AISquadBody->SetCommandState(EAIUnitCommandState::DAMAGE);
		break;
	case EEnemyState::DIE:
		AISquadBody->SetCommandState(EAIUnitCommandState::DIE);
		AISquadBody->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AISquadController->FCallback_AIController_MoveCompleted.RemoveAll(this);
		AISquadAnimInstance->PlayDieMontage();
		break;
	default:
		break;
	}
}
void UAISquadFSMComponent::TickIdle(const float& DeltaTime)
{
	if (GetIsAttacking()&&GetTarget())
	{
		//Idle 상태에서 공격시 상대를 바라보게 회전 후 공격 
		float LookRotator = AISquadBody->GetLookTargetAngle(GetTarget()->GetActorLocation()) + BaseAttackRotatorYaw;
		//if (fabs(AISquadAnimInstance->GetAimYaw() - LookRotator) <= 20)
		//{
			//RotateUpperbodyToTarget(DeltaTime);
		//}
		if (LookRotator >= -30 && LookRotator <= 30)
		{
			//상체 애니메이션 타겟을 향하게 회전
			float newYaw = FMath::Lerp(AISquadAnimInstance->GetAimYaw(),LookRotator, DeltaTime*5.0f);
			AISquadAnimInstance->SetAimYaw(newYaw);
			//공격한 뒤 2초가 지나야 다시 공격이 가능하다.
			//적을 조준하고 있는 상태여야 공격이 가능하다.
			if (GetAttackCurrentTime() >= AttackCoolTime && fabs(LookRotator - newYaw)< 0.2)
			{
				StartAttack();	
			}
			//if (LookRotator.Pitch >= -90 && LookRotator.Pitch <= 90)
				//AISquadAnimInstance->SetAimPitch(LookRotator.Pitch);
		}
		else
		{
			TurnCanLookTarget(DeltaTime);
		}
	}
}
void UAISquadFSMComponent::TickMove(const float& DeltaTime)
{
	if (GetIsAttacking()&&GetTarget())
	{
		RotateUpperbodyToTarget(DeltaTime);
	}
}
void UAISquadFSMComponent::TickAttack(const float& DeltaTime)
{
	
}
void UAISquadFSMComponent::TickDamage(const float& DeltaTime)
{
}
void UAISquadFSMComponent::TickDie(const float& DeltaTime)
{

}

void UAISquadFSMComponent::StartAttack()
{
	SetAttackCurrentTime(0.f);
	AISquadAnimInstance->PlayFireMontage();
	/*FTimerHandle StopAttackHandle;
	GetWorld()->GetTimerManager().SetTimer(StopAttackHandle, this, &UAISquadFSMComponent::EndAttack, 2.0f, false);*/
}
void UAISquadFSMComponent::EndAttack()
{
	AISquadAnimInstance->StopFireMontage();
}
void UAISquadFSMComponent::OnMoveCompleted(EPathFollowingResult::Type Result)
{
    // 이동이 성공적으로 완료된 경우에만 다음 지점으로 이동
    if (Result == EPathFollowingResult::Success)
    {
        CurrentPathPointIndex++;
		 
		 if (CurrentPathPointIndex < (PathVectorArray.Num()))
		{
			//// 다음 경로 지점으로 이동
			FVector NextPoint;
			if (CurrentPathPointIndex == PathVectorArray.Num() - 1)
			{
				NextPoint = PathVectorArray[CurrentPathPointIndex];
				AISquadController->MoveToLocation(NextPoint);
			}
			else
			{
				NextPoint = PathVectorArray[CurrentPathPointIndex] + SquadPosition;
				AISquadController->MoveToLocation(NextPoint, 500.0f);
			}
		}
		else
		{
			SetState(EEnemyState::IDLE);
		}
	}
	else if (CurrentPathPointIndex < (PathVectorArray.Num()-1))
	{
		AISquadController->MoveToLocation(PathVectorArray[CurrentPathPointIndex]);
	}
	else
	{
		SetState(EEnemyState::IDLE);
		AISquadController->FCallback_AIController_MoveCompleted.RemoveAll(this);
		//SquadManager가 이동 불가능한 위치를 주었다면 델리게이트 호출
		if(AISquadBody->FDelFailToDestination.IsBound())
			AISquadBody->FDelFailToDestination.Execute(AISquadBody->GetMySquadNumber());
		//SetState(EEnemyState::IDLE);
	}
}

void UAISquadFSMComponent::MovePathAsync(TArray<FVector>& NavPathArray)
{
	SetState(EEnemyState::MOVE);
	PathVectorArray = NavPathArray;
	CurrentPathPointIndex  = 1;
	AISquadController->FCallback_AIController_MoveCompleted.RemoveAll(this);
	
  //남은 경로 지점이 있는지 확인
    if (CurrentPathPointIndex < (PathVectorArray.Num()))
    {
		// 이동 완료 후 다시 OnMoveCompleted 호출
		AISquadController->FCallback_AIController_MoveCompleted.AddUFunction(this, FName("OnMoveCompleted"));
		// 다음 경로 지점으로 이동
		FVector NextPoint;
		if (CurrentPathPointIndex == PathVectorArray.Num() - 1)
		{
			NextPoint = PathVectorArray[CurrentPathPointIndex];
			AISquadController->MoveToLocation(NextPoint);
		}
		else
		{
			NextPoint = PathVectorArray[CurrentPathPointIndex] + SquadPosition;
			AISquadController->MoveToLocation(NextPoint, 500.0f);
		}
    }
    else
    {
		SetState(EEnemyState::IDLE);
        UE_LOG(LogTemp, Warning, TEXT("Fail Reached final destination!"));
    }
}


void UAISquadFSMComponent::MoveToTarget()
{
	FVector dir = GetTarget()->GetActorLocation() - AISquadBody->GetActorLocation();
	float dist = dir.Size();
	//Me->AddMovementInput(dir.GetSafeNormal());

	FVector destinataion = GetTarget()->GetActorLocation();

	auto* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(destinataion);
	MoveRequest.SetAcceptanceRadius(50);

	FPathFindingQuery Query;
	AISquadController->BuildPathfindingQuery(MoveRequest , Query);
	FPathFindingResult r = ns->FindPathSync(Query);

	// 만약 목적지가 길 위에있다면
	if (r.Result == ENavigationQueryResult::Success)
	{
		// 목적지를 향해서 이동하고싶다.
		AISquadController->MoveToLocation(destinataion);
		// 만약 목적지와의 거리가 공격 가능거리라면
		if (nullptr != GetTarget() && dist < AttackDistance )
		{
			// 공격상태로 전이하고싶다.
			SetState(EEnemyState::ATTACK);
		}
	}
	// 그렇지 않다면
	else
	{

	}
}


// Called when the game starts
void UAISquadFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	SetTarget(GetWorld()->GetFirstPlayerController()->GetPawn());

	// ...
	AISquadBody = Cast<AAISquad>(GetOwner());
	if (AISquadBody)
	{
		AISquadController = Cast<AAISquadController>(AISquadBody->GetController());
		AISquadAnimInstance = Cast<UAISquadAnimInstance>(AISquadBody->GetMesh()->GetAnimInstance());

		//공격 시작
		//SetIsAttacking(true);
		//AISquadAnimInstance->SetIsAttacking(true);
	}
}
void UAISquadFSMComponent::TurnCanLookTarget(const float& DeltaTime)
{
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(AISquadBody->GetActorLocation(),GetTarget()->GetActorLocation());
	FRotator NewRot = FMath::RInterpTo(AISquadBody->GetActorRotation(), PlayerRot, DeltaTime, 4.0f);
	AISquadBody->SetActorRotation(NewRot);
}
void UAISquadFSMComponent::RotateUpperbodyToTarget(const float& DeltaTime)
{
	if (nullptr != GetTarget())
	{
		float LookRotator = AISquadBody->GetLookTargetAngle(GetTarget()->GetActorLocation())+ BaseAttackRotatorYaw;
		
		if (LookRotator >= -90 && LookRotator <= 90)
		{
			//상체 애니메이션 타겟을 향하게 회전
			float newYaw = FMath::Lerp(AISquadAnimInstance->GetAimYaw(),LookRotator, DeltaTime*5.0f);
			AISquadAnimInstance->SetAimYaw(newYaw);
			//공격한 뒤 2초가 지나야 다시 공격이 가능하다.
			//적을 조준하고 있는 상태여야 공격이 가능하다.
			if (GetAttackCurrentTime() >= AttackCoolTime && fabs(LookRotator - newYaw)< 0.2)
			{
				StartAttack();	
			}
			//if (LookRotator.Pitch >= -90 && LookRotator.Pitch <= 90)
				//AISquadAnimInstance->SetAimPitch(LookRotator.Pitch);
		}
		else
		{
			AISquadController->FCallback_AIController_MoveCompleted.RemoveAll(this);
			AISquadController->StopMovement();
			SetState(EEnemyState::IDLE);

			/*if (GetAttackCurrentTime() < AttackCoolTime)
			{
				EndAttack();
			}

			float newYaw = FMath::Lerp(AISquadAnimInstance->GetAimYaw(),0,DeltaTime*10.0f);
			AISquadAnimInstance->SetAimYaw(newYaw);*/
		}
		/*	FHitResult OutHit;
			FVector Start = AISquadBody->GetGunMeshComp()->GetSocketLocation(TEXT("Muzzle"));
			FVector End = Target->GetActorLocation();
			ECollisionChannel TraceChannel = ECC_Camera;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(AISquadBody);
			Params.AddIgnoredComponent(AISquadBody->GetGunMeshComp());
			bool Result = GetWorld()->LineTraceSingleByChannel(OutHit,Start,End,TraceChannel,Params);
			if (Result)
			{
				DrawDebugLine(GetWorld(),Start,End,FColor::Red);
			}*/
	}
}


// Called every frame
void UAISquadFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//공격 쿨타임
	if(AISquadAnimInstance->GetIsAttacking())
		SetAttackCurrentTime(GetAttackCurrentTime() + DeltaTime);

	//FString myState = UEnum::GetValueAsString(GetCurrentState());
	//DrawDebugString(GetWorld() , GetOwner()->GetActorLocation() , myState , nullptr , FColor::Yellow , 0 , true , 1);

	switch ( GetCurrentState() )
	{
	case EEnemyState::IDLE:		TickIdle(DeltaTime);		break;
	case EEnemyState::MOVE:		TickMove(DeltaTime);		break;
	case EEnemyState::ATTACK:	TickAttack(DeltaTime);		break;
	case EEnemyState::DAMAGE:	TickDamage(DeltaTime);		break;
	case EEnemyState::DIE:		TickDie(DeltaTime);			break;
	}
	// ...
}

void UAISquadFSMComponent::SetIsAttacking(bool val, AActor* TargetActor)
{
	IsAttacking = val;

	//안보이는상태이고 공격중이면 보이게 만든다.
	if(AISquadBody->IsHidden() && IsAttacking)
		AISquadBody->AddViewCount();
	//보이는 상태이고 공격중이 아니라면 안보이게 만든다.
	else if (false == AISquadBody->IsHidden() && false == IsAttacking)
	{
		FTimerHandle HiddenHandle;
		GetWorld()->GetTimerManager().SetTimer(HiddenHandle, [&]()
		{
			AISquadBody->MinusViewCount();
		}, 2.5f, false);
	}
		
		

	Target = TargetActor;
	AISquadAnimInstance->SetIsAttacking(val);
	AISquadController->StopMovement();
}

FVector UAISquadFSMComponent::GetLastDestination()
{
	if(!PathVectorArray.IsEmpty())
		return PathVectorArray.Last();
	return FVector::ZeroVector;
}

