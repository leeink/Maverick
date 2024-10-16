// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadFSMComponent.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "XR_LSJ/AISquad.h"
#include "CollisionQueryParams.h"
#include "NavigationPath.h"
#include "XR_LSJ/AISquadController.h"

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

	// 다음 상태가 이동상태가 아니라면 Ai한테 멈추라고 하고싶다.
	if (NextState != EEnemyState::MOVE)
	{
		AISquadController->StopMovement();
	}

	// 상태가 바뀔때 무엇인가 초기화 하고싶다면 여기서 하세요.
	switch ( GetCurrentState() )
	{
	case EEnemyState::IDLE:
		break;
	case EEnemyState::MOVE:
		break;
	case EEnemyState::ATTACK:
		break;
	case EEnemyState::DAMAGE:
		break;
	case EEnemyState::DIE:
		break;
	default:
		break;
	}
}

void UAISquadFSMComponent::TickMove(const float& DeltaTime)
{

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
				AISquadController->MoveToLocation(NextPoint, 100.0f);
			}
		}
		else
		{
			SetState(EEnemyState::IDLE);
			UE_LOG(LogTemp, Warning, TEXT("Reached final destination 3! %d %s"),(int)Result,*AISquadBody->GetName());
		}
	}
	else
	{
		SetState(EEnemyState::IDLE);
		UE_LOG(LogTemp, Warning, TEXT("Failed final destination 3! %d %s"),(int)Result,*AISquadBody->GetName());
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
			AISquadController->MoveToLocation(NextPoint, 100.0f);
		}

        // 이동 완료 후 다시 OnMoveCompleted 호출
        AISquadController->FCallback_AIController_MoveCompleted.AddUFunction(this, FName("OnMoveCompleted"));
    }
    else
    {
		SetState(EEnemyState::IDLE);
        UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
    }
}


void UAISquadFSMComponent::MoveToTarget()
{
	FVector dir = Target->GetActorLocation() - AISquadBody->GetActorLocation();
	float dist = dir.Size();
	//Me->AddMovementInput(dir.GetSafeNormal());

	FVector destinataion = Target->GetActorLocation();

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
		if (nullptr != Target && dist < AttackDistance )
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

void UAISquadFSMComponent::TickIdle(const float& DeltaTime)
{
}
// Called when the game starts
void UAISquadFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AISquadBody = Cast<AAISquad>(GetOwner());
	if(AISquadBody)
		AISquadController = Cast<AAISquadController>(AISquadBody->GetController());
}




// Called every frame
void UAISquadFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString myState = UEnum::GetValueAsString(GetCurrentState());
	DrawDebugString(GetWorld() , GetOwner()->GetActorLocation() , myState , nullptr , FColor::Yellow , 0 , true , 1);

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

