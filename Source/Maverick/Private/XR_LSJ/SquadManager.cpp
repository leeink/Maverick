// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/SquadManager.h"
#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ActorComponent.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationPath.h"
// Sets default values
ASquadManager::ASquadManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    SquadPositionArray.Add(FVector(0, 0, 0));
	SquadPositionArray.Add(FVector(150, -200, 0));
	SquadPositionArray.Add(FVector(-150, -150, 0));
	SquadPositionArray.Add(FVector(-150, 150, 0));
	SquadPositionArray.Add(FVector(250, 0, 0));
	SquadPositionArray.Add(FVector(150, 200, 0));
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
    AttachToComponent(SquadArray[0]->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale);
    //AttachToActor(SquadArray[0],FAttachmentTransformRules::SnapToTargetIncludingScale);
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ASquadManager::CheckLocationForObject, 5.0f, true);
}

void ASquadManager::FindPath(const FVector& TargetLocation)
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if(nullptr == NavSystem)
        return;
    FVector StartLocation = SquadArray[0]->GetActorLocation();
    UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation);
    if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
       for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	   {
           FVector DirectionPosition = GetActorForwardVector()*SquadPositionArray[SquadCount].X+GetActorRightVector()*SquadPositionArray[SquadCount].Y;
           DirectionPosition.Z = 0;
           SquadArray[SquadCount]->FSMComp->SetSquadPosition(DirectionPosition);
		   SquadArray[SquadCount]->FSMComp->MovePathAsync(NavPath);
	   }
    }
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // 경로가 끊겼을때
    {
         
    }
}

void ASquadManager::FindObstructionPath(TArray<FVector>& TargetLocation)
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if(nullptr == NavSystem)
        return;
    FVector StartLocation = SquadArray[0]->GetActorLocation();
    UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation[0]);
    if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
       for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	   {
           NavPath->PathPoints.Last() = TargetLocation[SquadCount];
           FVector DirectionPosition = GetActorForwardVector()*SquadPositionArray[SquadCount].X+GetActorRightVector()*SquadPositionArray[SquadCount].Y;
           DirectionPosition.Z = 0;
           SquadArray[SquadCount]->FSMComp->SetSquadPosition(DirectionPosition);
		   SquadArray[SquadCount]->FSMComp->MovePathAsync(NavPath);
	   }
    }
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // 경로가 끊겼을때
    {
         
    }
}

void ASquadManager::CheckLocationForObject()
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

     // 결과 처리
    if (bHit) //목표지점에 오브젝트 존재 시 
    {
        //SquadArray[0]를 넣는게 아니라 다른 방법을 찾아보자
        //SquadArray를 탐색해서 있으면 넣자
        //분대장을 기준으로 할 예정
        ObstructionPoints = GetBoundingBoxPointsSortedByDistance(HitResult.GetActor(), 100.0f);
        FindObstructionPath(ObstructionPoints);
        ArrivalPoint *= -1;
        // 디버그용 박스 트레이스 시각화 (충돌 시 빨간색)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else //목표지점에 오브젝트가 없다면
    {
       FindPath(BoxStart);
       ArrivalPoint *= -1;
        // 디버그용 박스 트레이스 시각화 (충돌 없을 시 초록색)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
}
void ASquadManager::CheckLocationForObject(const FVector& TargetLocation)
{
       // 트레이스 시작과 끝 위치 설정
    FVector BoxStart = TargetLocation;
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

     // 결과 처리
    if (bHit) //목표지점에 오브젝트 존재 시 
    {
        //SquadArray[0]를 넣는게 아니라 다른 방법을 찾아보자
        //SquadArray를 탐색해서 있으면 넣자
        //분대장을 기준으로 할 예정
        ObstructionPoints = GetBoundingBoxPointsSortedByDistance(HitResult.GetActor(), 100.0f);
        FindObstructionPath(ObstructionPoints);

        // 디버그용 박스 트레이스 시각화 (충돌 시 빨간색)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else //목표지점에 오브젝트가 없다면
    {
       FindPath(TargetLocation);
        // 디버그용 박스 트레이스 시각화 (충돌 없을 시 초록색)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
}

// Called every frame
void ASquadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 일정 간격(50cm)으로 바운딩 박스 주위를 둘러싼 점들을 SquadManager와의 거리 기준으로 정렬
TArray<FVector> ASquadManager::GetBoundingBoxPointsSortedByDistance(AActor* TargetActor, float Interval /*= 50.0f*/)
{
    TArray<FVector> Points;
    TArray<float> PointLength;

    if (!TargetActor)
    {
        return Points;
    }

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
                PointLength.Add(FVector::Distance(BoundaryPoint,GetActorLocation()));
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