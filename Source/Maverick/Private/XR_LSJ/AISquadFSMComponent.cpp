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

	// �ִϸ��̼��� ���µ� ����ȭ �ϰ�ʹ�.
	//Anim->EnemyState = NextState;

	//CurrentTime = 0;

	// ���� ���°� �̵����°� �ƴ϶�� Ai���� ���߶�� �ϰ�ʹ�.
	if (NextState != EEnemyState::MOVE)
	{
		AISquadController->StopMovement();
	}

	// ���°� �ٲ� �����ΰ� �ʱ�ȭ �ϰ�ʹٸ� ���⼭ �ϼ���.
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
	//ArrivalPoint(������)�� �����Ǿ� �ִٸ� �������� ���� �̵�
	//������ ���� �߰�
	if(GetArrivalPoint()!=FVector::ZeroVector)
		MoveToArrivalPoint();
	//Target�� �����Ǿ� �ִٸ� ���ݻ����Ÿ� �ȱ��� �̵� �� ����
	else if (nullptr != Target)
		MoveToTarget();
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
    // �̵��� ���������� �Ϸ�� ��쿡�� ���� �������� �̵�
    if (Result == EPathFollowingResult::Success && PrePath == CurrentPath)
    {
        CurrentPathPointIndex++;
		 
        // ���� ��� ������ �ִ��� Ȯ��
        if (CurrentPathPointIndex < (CurrentPath->PathPoints.Num()))
        {
            // ���� ��� �������� �̵�
            FVector NextPoint = CurrentPath->PathPoints[CurrentPathPointIndex] + SquadPosition;
            AISquadController->MoveToLocation(NextPoint);

            // �̵� �Ϸ� �� �ٽ� OnMoveCompleted ȣ��
            AISquadController->FCallback_AIController_MoveCompleted.AddUFunction(this, FName("OnMoveCompleted"));
        }
        else
        {
			SetState(EEnemyState::IDLE);
            UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
        }
    }
}
void UAISquadFSMComponent::MovePathAsync(UNavigationPath* NavPath)
{
	SetState(EEnemyState::MOVE);
	CurrentPath = NavPath;
	CurrentPathPointIndex  = 1;
	AISquadController->FCallback_AIController_MoveCompleted.RemoveAll(this);
 // ���� ��� ������ �ִ��� Ȯ��
    if (CurrentPathPointIndex < (CurrentPath->PathPoints.Num()))
    {
        // ���� ��� �������� �̵�
        FVector NextPoint = CurrentPath->PathPoints[CurrentPathPointIndex] + SquadPosition;
        AISquadController->MoveToLocation(NextPoint);
		PrePath = CurrentPath;
        // �̵� �Ϸ� �� �ٽ� OnMoveCompleted ȣ��
        AISquadController->FCallback_AIController_MoveCompleted.AddUFunction(this, FName("OnMoveCompleted"));
    }
    else
    {
		SetState(EEnemyState::IDLE);
        UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
    }
}
void UAISquadFSMComponent::MoveToArrivalPoint()
{
	FVector dir = GetArrivalPoint() - AISquadBody->GetActorLocation();
	float dist = dir.Size();

	auto* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(GetArrivalPoint()); 
	MoveRequest.SetAcceptanceRadius(50);

	FPathFindingQuery Query;
	AISquadController->BuildPathfindingQuery(MoveRequest , Query);
	FPathFindingResult r = ns->FindPathSync(Query);


	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("IsSuccessful true")));


	if (r.IsPartial()) //�������� �� �� ���ٸ�
	{
		//

		if (r.Result == ENavigationQueryResult::Success)
		{
			// 
			AISquadController->MoveToLocation(GetArrivalPoint());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("IsPartial true %s"),*AISquadController->GetName()));
		}
		// �׷��� �ʴٸ�
		else
		{
			SetPatrolPoint(AISquadBody->GetActorLocation() , PatrolPointRadius , PatrolPoint);
		}
	}
	else //�������� �� �� �ִٸ�
	{
		// ���� �������� �� �����ִٸ�
		if (r.Result == ENavigationQueryResult::Success)
		{
			// �������� ���ؼ� �̵��ϰ�ʹ�.
			AISquadController->MoveToLocation(GetArrivalPoint());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("IsPartial false %s"),*AISquadController->GetName()));
		}
		// �׷��� �ʴٸ�
		else
		{
			SetPatrolPoint(AISquadBody->GetActorLocation() , PatrolPointRadius , PatrolPoint);
		}
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

	// ���� �������� �� �����ִٸ�
	if (r.Result == ENavigationQueryResult::Success)
	{
		// �������� ���ؼ� �̵��ϰ�ʹ�.
		AISquadController->MoveToLocation(destinataion);
		// ���� ���������� �Ÿ��� ���� ���ɰŸ����
		if (nullptr != Target && dist < AttackDistance )
		{
			// ���ݻ��·� �����ϰ�ʹ�.
			SetState(EEnemyState::ATTACK);
		}
	}
	// �׷��� �ʴٸ�
	else
	{
		SetPatrolPoint(AISquadBody->GetActorLocation() , PatrolPointRadius , PatrolPoint);
	}
}

bool UAISquadFSMComponent::SetPatrolPoint(FVector origin, float radius, FVector& dest)
{
	// ������ ������ ��ġ�� ���ϰ�ʹ�.
	auto* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool isSuccess = ns->GetRandomReachablePointInRadius(origin, radius, loc);
	// ���� �����ߴٸ�
	if ( isSuccess )
	{
		// �� ��ġ�� dest�� �����ϰ�ʹ�.
		dest = loc.Location;
	}
	return isSuccess;
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

