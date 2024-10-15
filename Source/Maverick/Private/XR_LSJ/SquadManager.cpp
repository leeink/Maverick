// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/SquadManager.h"
#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ActorComponent.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationPath.h"
#include "PhysicsEngine/ConvexElem.h"
#include "PhysicsEngine/BodySetup.h"
#include "Engine/StaticMesh.h"
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
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ASquadManager::CheckLocationForObject, 10.0f, true);
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
           NavPath->PathPoints.Last()+=DirectionPosition;
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
        //ObstructionPoints = GetBoundingBoxPointsSortedByDistance(HitResult.GetActor(), 100.0f);
        ObstructionPoints = GetSurfacePointsOnRotatedBoundingBox(HitResult.GetActor(), 100.0f);
        UE_LOG(LogTemp, Log, TEXT("ObstructionPoints(%d)"), ObstructionPoints.Num());
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

// 두 점 P1, P2 사이를 직선 방정식을 이용해 일정 간격으로 점을 생성
void ASquadManager::GeneratePointsBetweenTwoCorners(const FVector& P1, const FVector& P2, float Interval, TArray<FVector>& OutPoints)
{
    float Distance = FVector::Dist(P1, P2);
    int32 NumPoints = FMath::CeilToInt(Distance / Interval);  // 50cm 간격으로 몇 개의 점을 생성할지 계산

    for (int32 i = 0; i <= NumPoints; ++i)
    {
        float t = i / static_cast<float>(NumPoints);  // t는 0에서 1 사이를 일정하게 증가
		FVector Point = FMath::Lerp(P1, P2, t);       // P(t) = (1 - t) * P1 + t * P2
        Point.Z = SquadArray[0]->GetActorLocation().Z;
        OutPoints.Add(Point);                         // 점을 결과 배열에 추가
    }
}
// 일정 간격(50cm)으로 바운딩 박스 주위를 둘러싼 점들을 SquadManager와의 거리 기준으로 정렬
TArray<FVector> ASquadManager::GetSurfacePointsOnRotatedBoundingBox(AActor* TargetActor, float Interval /*= 50.0f*/)
{
    TArray<FVector> VertexArray;
    TArray<FVector> NewVertexArray;
    if (!TargetActor)
    {
        return NewVertexArray;  // 액터가 없으면 빈 배열 반환
    }

    // 액터의 바운딩 박스 계산
    FBox ActorBoundingBox = TargetActor->GetComponentsBoundingBox();
    

    UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComp)
	{
        UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();

        // 8개의 극단적 꼭지점을 계산할 변수들
        FVector3f MinX, MaxX, MinY, MaxY;
		FPositionVertexBuffer& VertexBuffer = StaticMesh->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
		MinX = MaxX = VertexBuffer.VertexPosition(0);
		MinY = MaxY = VertexBuffer.VertexPosition(0);
         
         for (uint32 Index = 0; Index < VertexBuffer.GetNumVertices(); ++Index)
		 {
			 FVector3f EachVector1 = StaticMesh->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer.VertexPosition(Index);

			 if (EachVector1.X < MinX.X) MinX = EachVector1;
			 if (EachVector1.X > MaxX.X) MaxX = EachVector1;
			 if (EachVector1.Y < MinY.Y) MinY = EachVector1;
			 if (EachVector1.Y > MaxY.Y) MaxY = EachVector1;

			 if (Index == 1)
				 UE_LOG(LogTemp, Log, TEXT("Index(%d) : (%s)"), Index, *EachVector1.ToString());
			 //if (Index == 1)
				// DrawDebugSphere(TargetActor->GetWorld(), EachVector, 10.0f, 12, FColor::Green, false, 5.0f);
		 }
          // 월드 변환 (위치, 회전, 스케일 적용)
         FTransform ActorTransform = TargetActor->GetActorTransform();

		 FVector Vertex1 = ActorTransform.TransformPosition(FVector(MinX.X, MinY.Y, 0));
		 FVector Vertex2 = ActorTransform.TransformPosition(FVector(MaxX.X, MinY.Y, 0));
		 FVector Vertex3 = ActorTransform.TransformPosition(FVector(MinX.X, MaxY.Y, 0));
		 FVector Vertex4 = ActorTransform.TransformPosition(FVector(MaxX.X, MaxY.Y, 0));
         
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X - 15.0f, MinY.Y - 15.0f, 0))); // 좌하단 앞쪽
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 15.0f, MinY.Y - 15.0f, 0))); // 우하단 앞쪽
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X - 15.0f, MaxY.Y + 15.0f, 0))); // 좌하단 뒤쪽
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 15.0f, MaxY.Y + 15.0f, 0))); // 우하단 뒤쪽

         VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X + (MaxX.X-MinX.X)/2, MinY.Y - 5.0f, 0))); // 좌하단 앞쪽
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X + (MaxX.X-MinX.X)/2, MaxY.Y + 5.0f, 0))); // 우하단 앞쪽
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X- 5.0f,  MinY.Y + (MaxY.Y- MinY.Y)/2, 0))); // 좌하단 뒤쪽
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 5.0f, MinY.Y + (MaxY.Y- MinY.Y)/2 , 0))); // 우하단 뒤쪽
         //내적을 한 후 가까운 방향의 꼭지점을 구한다.
         GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[1], 100.0f, NewVertexArray);
         GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[2], 100.0f, NewVertexArray);
         
         float TargetActorTopDirection = FVector::DotProduct(TargetActor->GetActorRightVector(), GetActorLocation());
         if (TargetActorTopDirection > 0)
         {
              //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[1], 100.0f, NewVertexArray);
              UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         }
         else
         {
            //GeneratePointsBetweenTwoCorners(VertexArray[2], VertexArray[3], 100.0f, NewVertexArray);
             UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         }
           
         float TargetActorRightDirection = FVector::DotProduct(TargetActor->GetActorForwardVector(), GetActorLocation());
         if (TargetActorRightDirection > 0)
         {
              //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[2], 100.0f, NewVertexArray);
              UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         }
         else
         {
            //GeneratePointsBetweenTwoCorners(VertexArray[1], VertexArray[3], 100.0f, NewVertexArray);
             UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         }
            
	}
    float DistanceVertex = 10000.f;
    for (int i = 4; i<VertexArray.Num(); i++)
    {
		UE_LOG(LogTemp, Log, TEXT("Index(%s)"), *VertexArray[i].ToString());
		DrawDebugSphere(TargetActor->GetWorld(), VertexArray[i], 10.0f, 12, FColor::Yellow, false, 5.0f);
        if (DistanceVertex>FVector::Distance(VertexArray[i], GetActorLocation()))
        {
            DistanceVertex = FVector::Distance(VertexArray[i], GetActorLocation());
            UE_LOG(LogTemp, Log, TEXT("DistanceVertex(%d)"), i);
            
        }
        UE_LOG(LogTemp, Log, TEXT("Vertex(%d)"), i);
    }
      for (int i = 0; i<NewVertexArray.Num(); i++)
    {
		UE_LOG(LogTemp, Log, TEXT("NewVertexArray(%s)"), *NewVertexArray[i].ToString());
		DrawDebugSphere(TargetActor->GetWorld(), NewVertexArray[i], 10.0f, 12, FColor::Yellow, false, 5.0f);

    }
    UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
    
    //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[1], 100.0f, NewVertexArray);
    //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[2], 100.0f, NewVertexArray);
    //GeneratePointsBetweenTwoCorners(VertexArray[2], VertexArray[3], 50.0f, NewVertexArray);
    //GeneratePointsBetweenTwoCorners(VertexArray[1], VertexArray[3], 50.0f, NewVertexArray);
    return NewVertexArray;
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
    FTransform ComponentTransform;
    ComponentArray.Append(ComponentSet.Array()); 

    //충돌한 액터의 FTransform 정보를 가진 컴포넌트를 가져오기
	for (UActorComponent* Component : ComponentArray)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Component))
		{
			if (PrimitiveComp->IsRegistered() && PrimitiveComp->IsCollisionEnabled())
			{
                ComponentTransform = PrimitiveComp->GetComponentTransform();
                FBox ComponentBounds = PrimitiveComp->Bounds.GetBox();

                // 유효한 바운딩 박스가 있는지 확인하고 바운딩 박스의 Min, Max 값 구하기
                if (ComponentBounds.IsValid)
                {
                    Bounds = ComponentBounds;
                    Min = Bounds.Min;
                    Max = Bounds.Max;
                    //Min = ComponentTransform.TransformPosition(Min);
                     //Max = ComponentTransform.TransformPosition(Max);
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