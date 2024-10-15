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
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // ��ΰ� ��������
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
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // ��ΰ� ��������
    {
         
    }
}

void ASquadManager::CheckLocationForObject()
{
       // Ʈ���̽� ���۰� �� ��ġ ����
    FVector BoxStart = ArrivalPoint;
    FVector BoxEnd = BoxStart; // X������ 1000 ���� ������ ��

    // �ڽ� ũ�� ���� (X, Y, Z ������)
    FVector BoxHalfSize = FVector(200.f, 200.f, 100.f);

    // �ڽ��� ȸ�� ���� (�ʿ信 ���� ȸ�� ����)
    FQuat Rotation = FQuat::Identity;

    // Ʈ���̽� ä�ΰ� �浹 �Ķ���� ����
    FCollisionQueryParams TraceParams(FName(TEXT("PawnMove")), false, this);

    // Ʈ���̽� ����� ������ FHitResult
    FHitResult HitResult;

    // �ڽ� Ʈ���̽� ����
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,         // �浹 ���
        BoxStart,             // ���� ��ġ
        BoxEnd,               // �� ��ġ
        Rotation,          // �ڽ��� ȸ��
        ECC_GameTraceChannel1,    // Ʈ���̽� ä�� (���ü� ä�� ���)
        FCollisionShape::MakeBox(BoxHalfSize),  // �ڽ� ��� ����
        TraceParams        // �߰� �Ķ����
    );

     // ��� ó��
    if (bHit) //��ǥ������ ������Ʈ ���� �� 
    {
        //SquadArray[0]�� �ִ°� �ƴ϶� �ٸ� ����� ã�ƺ���
        //SquadArray�� Ž���ؼ� ������ ����
        //�д����� �������� �� ����
        //ObstructionPoints = GetBoundingBoxPointsSortedByDistance(HitResult.GetActor(), 100.0f);
        ObstructionPoints = GetSurfacePointsOnRotatedBoundingBox(HitResult.GetActor(), 100.0f);
        UE_LOG(LogTemp, Log, TEXT("ObstructionPoints(%d)"), ObstructionPoints.Num());
        FindObstructionPath(ObstructionPoints);
        ArrivalPoint *= -1;
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 �� ������)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else //��ǥ������ ������Ʈ�� ���ٸ�
    {
       FindPath(BoxStart);
       ArrivalPoint *= -1;
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 ���� �� �ʷϻ�)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
}
void ASquadManager::CheckLocationForObject(const FVector& TargetLocation)
{
       // Ʈ���̽� ���۰� �� ��ġ ����
    FVector BoxStart = TargetLocation;
    FVector BoxEnd = BoxStart; // X������ 1000 ���� ������ ��

    // �ڽ� ũ�� ���� (X, Y, Z ������)
    FVector BoxHalfSize = FVector(200.f, 200.f, 100.f);

    // �ڽ��� ȸ�� ���� (�ʿ信 ���� ȸ�� ����)
    FQuat Rotation = FQuat::Identity;

    // Ʈ���̽� ä�ΰ� �浹 �Ķ���� ����
    FCollisionQueryParams TraceParams(FName(TEXT("PawnMove")), false, this);

    // Ʈ���̽� ����� ������ FHitResult
    FHitResult HitResult;

    // �ڽ� Ʈ���̽� ����
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,         // �浹 ���
        BoxStart,             // ���� ��ġ
        BoxEnd,               // �� ��ġ
        Rotation,          // �ڽ��� ȸ��
        ECC_GameTraceChannel1,    // Ʈ���̽� ä�� (���ü� ä�� ���)
        FCollisionShape::MakeBox(BoxHalfSize),  // �ڽ� ��� ����
        TraceParams        // �߰� �Ķ����
    );

     // ��� ó��
    if (bHit) //��ǥ������ ������Ʈ ���� �� 
    {
        //SquadArray[0]�� �ִ°� �ƴ϶� �ٸ� ����� ã�ƺ���
        //SquadArray�� Ž���ؼ� ������ ����
        //�д����� �������� �� ����
        ObstructionPoints = GetBoundingBoxPointsSortedByDistance(HitResult.GetActor(), 100.0f);
        FindObstructionPath(ObstructionPoints);

        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 �� ������)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else //��ǥ������ ������Ʈ�� ���ٸ�
    {
       FindPath(TargetLocation);
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 ���� �� �ʷϻ�)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
}

// Called every frame
void ASquadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// �� �� P1, P2 ���̸� ���� �������� �̿��� ���� �������� ���� ����
void ASquadManager::GeneratePointsBetweenTwoCorners(const FVector& P1, const FVector& P2, float Interval, TArray<FVector>& OutPoints)
{
    float Distance = FVector::Dist(P1, P2);
    int32 NumPoints = FMath::CeilToInt(Distance / Interval);  // 50cm �������� �� ���� ���� �������� ���

    for (int32 i = 0; i <= NumPoints; ++i)
    {
        float t = i / static_cast<float>(NumPoints);  // t�� 0���� 1 ���̸� �����ϰ� ����
		FVector Point = FMath::Lerp(P1, P2, t);       // P(t) = (1 - t) * P1 + t * P2
        Point.Z = SquadArray[0]->GetActorLocation().Z;
        OutPoints.Add(Point);                         // ���� ��� �迭�� �߰�
    }
}
// ���� ����(50cm)���� �ٿ�� �ڽ� ������ �ѷ��� ������ SquadManager���� �Ÿ� �������� ����
TArray<FVector> ASquadManager::GetSurfacePointsOnRotatedBoundingBox(AActor* TargetActor, float Interval /*= 50.0f*/)
{
    TArray<FVector> VertexArray;
    TArray<FVector> NewVertexArray;
    if (!TargetActor)
    {
        return NewVertexArray;  // ���Ͱ� ������ �� �迭 ��ȯ
    }

    // ������ �ٿ�� �ڽ� ���
    FBox ActorBoundingBox = TargetActor->GetComponentsBoundingBox();
    

    UStaticMeshComponent* StaticMeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComp)
	{
        UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();

        // 8���� �ش��� �������� ����� ������
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
          // ���� ��ȯ (��ġ, ȸ��, ������ ����)
         FTransform ActorTransform = TargetActor->GetActorTransform();

		 FVector Vertex1 = ActorTransform.TransformPosition(FVector(MinX.X, MinY.Y, 0));
		 FVector Vertex2 = ActorTransform.TransformPosition(FVector(MaxX.X, MinY.Y, 0));
		 FVector Vertex3 = ActorTransform.TransformPosition(FVector(MinX.X, MaxY.Y, 0));
		 FVector Vertex4 = ActorTransform.TransformPosition(FVector(MaxX.X, MaxY.Y, 0));
         
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X - 15.0f, MinY.Y - 15.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 15.0f, MinY.Y - 15.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X - 15.0f, MaxY.Y + 15.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 15.0f, MaxY.Y + 15.0f, 0))); // ���ϴ� ����

         VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X + (MaxX.X-MinX.X)/2, MinY.Y - 5.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X + (MaxX.X-MinX.X)/2, MaxY.Y + 5.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X- 5.0f,  MinY.Y + (MaxY.Y- MinY.Y)/2, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 5.0f, MinY.Y + (MaxY.Y- MinY.Y)/2 , 0))); // ���ϴ� ����
         //������ �� �� ����� ������ �������� ���Ѵ�.
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
// ���� ����(50cm)���� �ٿ�� �ڽ� ������ �ѷ��� ������ SquadManager���� �Ÿ� �������� ����
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

    //�浹�� ������ FTransform ������ ���� ������Ʈ�� ��������
	for (UActorComponent* Component : ComponentArray)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Component))
		{
			if (PrimitiveComp->IsRegistered() && PrimitiveComp->IsCollisionEnabled())
			{
                ComponentTransform = PrimitiveComp->GetComponentTransform();
                FBox ComponentBounds = PrimitiveComp->Bounds.GetBox();

                // ��ȿ�� �ٿ�� �ڽ��� �ִ��� Ȯ���ϰ� �ٿ�� �ڽ��� Min, Max �� ���ϱ�
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


    //��ǥ���� ���� �ȿ� �ִ� ���� Interval �������� ���� ����
    for (float X = Min.X - 10; X <= Max.X; X += Interval)
    {
        for (float Y = Min.Y - 10; Y <= Max.Y; Y += Interval)
        {
		    // ��迡 �ش��ϴ� ���� �߰� (��, �ڽ��� ǥ�鿡 �ִ� ���鸸)
            if ((X <= Min.X) || X >= Max.X || Y <= Min.Y || Y > Max.Y - 50)
            {
                FVector BoundaryPoint = FVector(X, Y, SquadArray[0]->GetActorLocation().Z);
          
                Points.Add(BoundaryPoint);
                //Points index�� �ش��ϴ� MyActor�� �Ÿ��� ���Ͽ� PointLength�� �ִ´�.
                PointLength.Add(FVector::Distance(BoundaryPoint,GetActorLocation()));
            }
        }
    }

    //Points �߿� ���� ����� 6����Ʈ�� ã�Ƽ� return�Ѵ�.
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
                // n ��°�� �ְ� �Ǹ� n��°���� max���� ��ĭ�� �з����� �Ѵ�.
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