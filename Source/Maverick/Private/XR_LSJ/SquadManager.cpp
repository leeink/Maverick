// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/SquadManager.h"
#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ActorComponent.h"

// Sets default values
ASquadManager::ASquadManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SquadPositionArray.Add(FVector(0, 100, 0));
	SquadPositionArray.Add(FVector(0, -100, 0));
	SquadPositionArray.Add(FVector(0, 0, 0));
	SquadPositionArray.Add(FVector(-100, 0, 0));
	SquadPositionArray.Add(FVector(-100, 100, 0));
	SquadPositionArray.Add(FVector(-100, -100, 0));

	MaxSpawnCount = 6;
}

// Called when the game starts or when spawned
void ASquadManager::BeginPlay()
{
	Super::BeginPlay();
	for (int SpawnCount = 0; SpawnCount < MaxSpawnCount; SpawnCount++)
	{
		SquadArray.Add(GetWorld()->SpawnActor<AAISquad>(SpawnSquadPactory, GetActorLocation() + SquadPositionArray[SpawnCount], GetActorRotation()));
		SquadArray[SpawnCount]->SetMySquadNumber(SpawnCount + 1);
	}
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ASquadManager::TestMove, 5.0f, true);
}
//현재 위치에서 가야할 위치가 위인지 아래인지
bool ASquadManager::UpDirectionPoint(AActor* BaseActor,FVector Point)
{
    float Direction = FVector::DotProduct (BaseActor->GetActorForwardVector ( ) , Point);
	if ( Direction > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
void ASquadManager::TestMove()
{
	   // 트레이스 시작과 끝 위치 설정
    FVector BoxStart = ArrivalPoint;
    FVector BoxEnd = BoxStart; // X축으로 1000 유닛 떨어진 곳

    // 박스 크기 설정 (X, Y, Z 반지름)
    FVector BoxHalfSize = FVector(200.f, 200.f, 100.f);

    // 박스의 회전 설정 (필요에 따라 회전 적용)
    FQuat Rotation = FQuat::Identity;

    // 트레이스 채널과 충돌 파라미터 설정
    FCollisionQueryParams TraceParams(FName(TEXT("PawnMove")), false, this);

    // 트레이스 결과를 저장할 FHitResult
    FHitResult HitResult;

    // 박스 트레이스 수행
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,         // 충돌 결과
        BoxStart,             // 시작 위치
        BoxEnd,               // 끝 위치
        Rotation,          // 박스의 회전
        ECC_GameTraceChannel1,    // 트레이스 채널 (가시성 채널 사용)
        FCollisionShape::MakeBox(BoxHalfSize),  // 박스 모양 설정
        TraceParams        // 추가 파라미터
    );

    bool UpDirection = false;

    // 결과 처리
    if (bHit)
    {
        
        FBox Bounds = HitResult.GetActor()->GetComponentsBoundingBox();
        //SquadArray[0]를 넣는게 아니라 다른 방법을 찾아보자
        //SquadArray를 탐색해서 있으면 넣자
        for (AAISquad* SquadPawn : SquadArray)
        {
            TestPoint = GetBoundingBoxPointsSortedByDistance(SquadPawn, HitResult.GetActor(), 100.0f);
            UpDirection = UpDirectionPoint(SquadPawn,TestPoint[0]);
            break;
        }
        if (UpDirection)
        {
        	for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
			{
				SquadArray[SquadCount]->FSMComp->SetArrivalPoint(TestPoint[SquadCount]);
				SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
			}
        }
        else
        {
            for (int SquadCount = SquadArray.Num() - 1; SquadCount >= 0 ; SquadCount--)
			{
				SquadArray[SquadCount]->FSMComp->SetArrivalPoint(TestPoint[SquadCount]);
				SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
			}
        }
	
        // 디버그용 박스 트레이스 시각화 (충돌 시 빨간색)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else
    {
        UpDirection = UpDirectionPoint(SquadArray[0], ArrivalPoint);
        if (UpDirection)
        {
        	for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
			{
				SquadArray[SquadCount]->FSMComp->SetArrivalPoint(ArrivalPoint+SquadPositionArray[SquadCount]);
				SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
			}
         
        }
        else
        {
            for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
			{
				SquadArray[SquadCount]->FSMComp->SetArrivalPoint(ArrivalPoint + SquadPositionArray[SquadArray.Num() - 1 - SquadCount]);
				SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
			}
        }
        // 디버그용 박스 트레이스 시각화 (충돌 없을 시 초록색)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
    //LineTrace로 충돌된 물체가 있는지 확인해서 그 물체의 경계값에 서기 
 //   FHitResult OutHit;
	//FVector Start = ArrivalPoint + FVector(0,0,100);
	//FVector End = ArrivalPoint + FVector(0,0,-100);
	//ECollisionChannel TraceChannel = ECollisionChannel::ECC_GameTraceChannel1; //PawnMove //Block 으로 해야 함
	//FCollisionQueryParams Params;

	////목적지를 가린 액터의 테두리로 이동
	//if (GetWorld()->LineTraceSingleByChannel(
	//	OutHit,
	//	Start,
	//	End,
	//	TraceChannel,
	//	Params))
	//{

	//	FBox Bounds = OutHit.GetActor()->GetComponentsBoundingBox();
 //       //SquadArray[0]를 넣는게 아니라 다른 방법을 찾아보자
 //       //SquadArray를 탐색해서 있으면 넣자
	//	TestPoint = GetBoundingBoxPointsSortedByDistance(SquadArray[0], OutHit.GetActor(), 100.0f);
	//	for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	//	{
	//		SquadArray[SquadCount]->FSMComp->SetArrivalPoint(TestPoint[SquadCount]);
	//		SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
	//	}
	//}
	//else
	//	for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	//	{
	//		SquadArray[SquadCount]->FSMComp->SetArrivalPoint(ArrivalPoint + SquadPositionArray[SquadCount]);
	//		SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
	//	}
	ArrivalPoint*=(-1);
}
// Called every frame
void ASquadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 일정 간격(50cm)으로 바운딩 박스 주위를 둘러싼 점들을 MyActor와의 거리 기준으로 정렬
TArray<FVector> ASquadManager::GetBoundingBoxPointsSortedByDistance(AActor* MyActor, AActor* TargetActor, float Interval /*= 50.0f*/)
{
    TArray<FVector> Points;
    TArray<float> PointLength;

    if (!TargetActor)
    {
        return Points;
    }

    // 액터의 바운딩 박스 계산
    //FBox Bounds = TargetActor->GetComponentsBoundingBox();
    //FVector Min = Bounds.Min;
    //FVector Max = Bounds.Max;
    FBox Bounds;
    FVector Min;
    FVector Max;

    TSet<UActorComponent*> ComponentSet = TargetActor->GetComponents();
    TArray<UActorComponent*> ComponentArray;
    ComponentArray.Append(ComponentSet.Array()); 

    //충돌한 액터의 FTransform 정보를 가진 컴포넌트를 가져오기
	for (UActorComponent* Component : ComponentArray)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Component))
		{
			if (PrimitiveComp->IsRegistered() && PrimitiveComp->IsCollisionEnabled())
			{
                FTransform ComponentTransform = PrimitiveComp->GetComponentTransform();
                FBox ComponentBounds = PrimitiveComp->Bounds.GetBox();

                // 유효한 바운딩 박스가 있는지 확인하고 바운딩 박스의 Min, Max 값 구하기
                if (ComponentBounds.IsValid)
                {
                    Bounds = ComponentBounds;
                    Min = Bounds.Min;
                    Max = Bounds.Max;
                    break;
                }
			}
		}
	}


    //목표지점 공간 안에 있는 축을 Interval 간격으로 점을 생성
    for (float X = Min.X - 10; X <= Max.X; X += Interval)
    {
        for (float Y = Min.Y - 10; Y <= Max.Y; Y += Interval)
        {
		    // 경계에 해당하는 점만 추가 (즉, 박스의 표면에 있는 점들만)
            if ((X <= Min.X) || X >= Max.X || Y <= Min.Y || Y > Max.Y - 50)
            {
                FVector BoundaryPoint = FVector(X, Y, SquadArray[0]->GetActorLocation().Z);
                Points.Add(BoundaryPoint);
                //Points index에 해당하는 MyActor와 거리를 구하여 PointLength에 넣는다.
                PointLength.Add(FVector::Distance(BoundaryPoint,MyActor->GetActorLocation()));
            }
        }
    }

    //Points 중에 가장 가까운 6포인트를 찾아서 return한다.
    TArray<FVector> ClosePoints;
    ClosePoints.Init(FVector::ZeroVector,6);
    TArray<float> CloseLength;
    CloseLength.Init(MaxMoveLength,6);
     for (int PointLengthCount =0; PointLengthCount<PointLength.Num(); PointLengthCount++)
    {
        for (int CloseLengthCount =0; CloseLengthCount<CloseLength.Num(); CloseLengthCount++)
        {
            if (CloseLength[CloseLengthCount] > PointLength[PointLengthCount])
            {
                // n 번째에 넣게 되면 n번째부터 max까지 한칸씩 밀려나게 한다.
                for (int currentCount = CloseLength.Num() - 1; currentCount>CloseLengthCount; currentCount--)
				{
					ClosePoints[currentCount] = ClosePoints[currentCount - 1];
					CloseLength[currentCount] = CloseLength[currentCount - 1];
				}
                ClosePoints[CloseLengthCount] = Points[PointLengthCount];
                CloseLength[CloseLengthCount] = PointLength[PointLengthCount];
                break;
            }
        }
    }

    return ClosePoints;
}