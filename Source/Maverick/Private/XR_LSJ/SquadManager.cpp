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

    CurrentSquadCount = MaxSpawnCount;
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
        TArray<FVector> ArrayLocation;
        ArrayLocation = NavPath->PathPoints;
       for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	   {
           FVector DirectionPosition = GetActorForwardVector()*SquadPositionArray[SquadCount].X+GetActorRightVector()*SquadPositionArray[SquadCount].Y;
           DirectionPosition.Z = 0;
           ArrayLocation.Last()+=DirectionPosition;
           SquadArray[SquadCount]->FSMComp->SetSquadPosition(DirectionPosition);
		   SquadArray[SquadCount]->FSMComp->MovePathAsync(ArrayLocation);
	   }
    }
    else if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // ��ΰ� ��������
    {
         
    }
}

void ASquadManager::FindObstructionPath(TArray<FVector>& TargetLocation)
{
    if(TargetLocation.IsEmpty())
        return;
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if(nullptr == NavSystem)
        return;
    FVector StartLocation = SquadArray[0]->GetActorLocation();
   UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation[0]);
    
    if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
        TArray<FVector> ArrayLocation;
        ArrayLocation = NavPath->PathPoints;
       for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	   {
           ArrayLocation.Last() = TargetLocation[SquadCount];
           FVector DirectionPosition = GetActorForwardVector()*SquadPositionArray[SquadCount].X+GetActorRightVector()*SquadPositionArray[SquadCount].Y;
           DirectionPosition.Z = 0;
           SquadArray[SquadCount]->FSMComp->SetSquadPosition(DirectionPosition);
		   SquadArray[SquadCount]->FSMComp->MovePathAsync(ArrayLocation);
	   }
    }
    else //if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // ��ΰ� ��������
    {
         UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
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
        ObstructionPoints = GetSurfacePointsOnRotatedBoundingBox(HitResult.GetActor(), 100.0f);
        UE_LOG(LogTemp, Log, TEXT("ObstructionPoints(%d)"), ObstructionPoints.Num());
        FindObstructionPath(ObstructionPoints);
        ArrivalPoint *= -1;
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
// �д�� ����ŭ FVector �����
void ASquadManager::MakeObstructionPoint(AActor* TargetActor, TArray<FVector>& OutPoints, EObstructionDirection DirectionNum)
{
    if (OutPoints.Num() == 1 || OutPoints.Num()<CurrentSquadCount) //�ӽ� ��ġ�� �д������ OutPoints.Num()�� ������ ��ġ�� �߰��Ѵ�.
    {
        FVector Temp = OutPoints[0];
        FVector BaseForwardVector;
        FVector BaseRightVector;
        if (DirectionNum == EObstructionDirection::Left) //�������� ������ �ٶ󺻴�.
        {
            BaseForwardVector = TargetActor->GetActorRightVector();
            BaseRightVector = (-1.f)*TargetActor->GetActorForwardVector();
        }
        else if (DirectionNum == EObstructionDirection::Right) //�������� ������ �ٶ󺻴�.
        {
            BaseForwardVector = (-1.f)*TargetActor->GetActorRightVector();
            BaseRightVector = TargetActor->GetActorForwardVector();
        }        
         else if (DirectionNum == EObstructionDirection::Down) //�ϴܿ��� ����� �ٶ󺻴�.
        {
            BaseForwardVector = TargetActor->GetActorForwardVector();
            BaseRightVector = TargetActor->GetActorRightVector();
        }
         else //��ܿ��� �ϴ��� �ٶ󺻴�.
        {
            BaseForwardVector = (-1.f)*TargetActor->GetActorForwardVector();
            BaseRightVector = (-1.f)*TargetActor->GetActorRightVector();
        }
        OutPoints.Init(FVector::ZeroVector, MaxSpawnCount);
        OutPoints[5]= Temp;
        OutPoints[0] = OutPoints[5] + (-1) *BaseForwardVector*SquadPositionArray[5].X;
        DrawDebugSphere(TargetActor->GetWorld(), OutPoints[0], 10.0f, 12, FColor::Black, false, 5.0f);
        for (int SquadCount = 1; SquadCount < SquadArray.Num(); SquadCount++)
	    {
			FVector DirectionPosition = BaseForwardVector * SquadPositionArray[SquadCount].X + BaseRightVector * SquadPositionArray[SquadCount].Y;
			DirectionPosition.Z = 0;
			OutPoints[SquadCount]=(OutPoints[0]+DirectionPosition);// += SquadPositionArray[SquadCount]);
            UE_LOG(LogTemp, Log, TEXT("NewVertexArray(%s)"), *OutPoints[SquadCount].ToString());
		    DrawDebugSphere(TargetActor->GetWorld(), OutPoints[SquadCount], 10.0f, 12, FColor::Blue, false, 5.0f);
        }
    }
    else if (OutPoints.Num() == 2)
    {
        
    }
}
// �� �� P1, P2 ���̸� ���� �������� �̿��� ���� �������� ���� ����
void ASquadManager::GeneratePointsBetweenTwoCorners(const FVector& P1, const FVector& P2, float Interval, TArray<FVector>& OutPoints)
{
    float Distance = FVector::Dist(P1, P2);
    int32 NumPoints = FMath::CeilToInt(Distance / Interval);  // 50cm �������� �� ���� ���� �������� ���

    for (int32 i = 1; i <= NumPoints-1; ++i)
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

		 FVector FrontLeftVertex = ActorTransform.TransformPosition(FVector(    MinX.X - 20.0f, MinY.Y - 20.0f, 0)); // ���ϴ� ����
		 FVector FrontRightVertex = ActorTransform.TransformPosition(FVector(   MaxX.X + 20.0f, MinY.Y - 20.0f, 0)); // ���ϴ� ����
		 FVector BackLeftVertex = ActorTransform.TransformPosition(FVector(     MinX.X - 20.0f, MaxY.Y + 20.0f, 0)); // ���ϴ� ����
		 FVector BackRightVertex = ActorTransform.TransformPosition(FVector(    MaxX.X + 20.0f, MaxY.Y + 20.0f, 0)); // ���ϴ� ����
         
         //�¿���� ������
		 VertexArray.Add(FrontLeftVertex); // ���ϴ� ����
		 VertexArray.Add(FrontRightVertex); // ���ϴ� ����
		 VertexArray.Add(BackLeftVertex); // ���ϴ� ����
		 VertexArray.Add(BackRightVertex); // ���ϴ� ����

         //�¿���� ����
         VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X + (MaxX.X-MinX.X)/2, MinY.Y - 5.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X + (MaxX.X-MinX.X)/2, MaxY.Y + 5.0f, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MinX.X- 5.0f,  MinY.Y + (MaxY.Y- MinY.Y)/2, 0))); // ���ϴ� ����
		 VertexArray.Add(ActorTransform.TransformPosition(FVector(MaxX.X + 5.0f, MinY.Y + (MaxY.Y- MinY.Y)/2 , 0))); // ���ϴ� ����
         
         //TargetActor�� SquadManager������ ����� ���⿡ �ش��ϴ� ��ġ�� ���Ѵ�.
         EObstructionDirection ObstructionDirection;
         float DirectionVertexDistance = MaxMoveLength;

         for (int DirectionVertexCount = 4; DirectionVertexCount < VertexArray.Num(); DirectionVertexCount++)
         {
             float ActorToVertexDistance = FVector::Distance(VertexArray[DirectionVertexCount], GetActorLocation());
             if (DirectionVertexDistance > ActorToVertexDistance)
             {
                DirectionVertexDistance = ActorToVertexDistance;
                ObstructionDirection = (EObstructionDirection)(DirectionVertexCount-4);
             }
         }
         switch (ObstructionDirection)
         {
         case EObstructionDirection::Left: //VertexArray[4] : ���� ����
            GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[1], 100.0f, NewVertexArray);
             break;
         case EObstructionDirection::Right: //VertexArray[5] : ���� ����
            GeneratePointsBetweenTwoCorners(VertexArray[2], VertexArray[3], 100.0f, NewVertexArray);
             break;
         case EObstructionDirection::Down: //VertexArray[6] : �ϴ� ����
            GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[2], 100.0f, NewVertexArray);
             break;
         case EObstructionDirection::Up: //VertexArray[7] : ��� ����
            GeneratePointsBetweenTwoCorners(VertexArray[1], VertexArray[3], 100.0f, NewVertexArray);
             break;
         default:
             break;
         }
             //���� ��ġ�� �д�� ������ ���ٸ� �д�� ��ġ�� �������ش�.
		 if (NewVertexArray.Num() < CurrentSquadCount)
			 MakeObstructionPoint(TargetActor, NewVertexArray, ObstructionDirection);
         //������ �� �� ����� ������ �������� ���Ѵ�.
         //VertexArray[4] : ���� ����
         //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[1], 100.0f, NewVertexArray);
         //VertexArray[5] : ���� ����
         //GeneratePointsBetweenTwoCorners(VertexArray[2], VertexArray[3], 100.0f, NewVertexArray);
         //VertexArray[6] : �ϴ� ����
         //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[2], 100.0f, NewVertexArray);
         //VertexArray[7] : ��� ����
         //GeneratePointsBetweenTwoCorners(VertexArray[1], VertexArray[3], 100.0f, NewVertexArray);

         //float TargetActorTopDirection = FVector::DotProduct(TargetActor->GetActorRightVector(), GetActorLocation());
         //if (TargetActorTopDirection > 0)
         //{
         //     //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[1], 100.0f, NewVertexArray);
         //     UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         //}
         //else
         //{
         //   //GeneratePointsBetweenTwoCorners(VertexArray[2], VertexArray[3], 100.0f, NewVertexArray);
         //    UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         //}
         //  
         //float TargetActorRightDirection = FVector::DotProduct(TargetActor->GetActorForwardVector(), GetActorLocation());
         //if (TargetActorRightDirection > 0)
         //{
         //     //GeneratePointsBetweenTwoCorners(VertexArray[0], VertexArray[2], 100.0f, NewVertexArray);
         //     UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         //}
         //else
         //{
         //   //GeneratePointsBetweenTwoCorners(VertexArray[1], VertexArray[3], 100.0f, NewVertexArray);
         //    UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
         //}
            
	}
  //  float DistanceVertex = 10000.f;
  //  int j = 4;
  //  for (int i = 4; i<VertexArray.Num(); i++)
  //  {
		//UE_LOG(LogTemp, Log, TEXT("Index(%s)"), *VertexArray[i].ToString());
		//DrawDebugSphere(TargetActor->GetWorld(), VertexArray[i], 10.0f, 12, FColor::Yellow, false, 5.0f);
  //      if (DistanceVertex>FVector::Distance(VertexArray[i], GetActorLocation()))
  //      {
  //          DistanceVertex = FVector::Distance(VertexArray[i], GetActorLocation());
  //          UE_LOG(LogTemp, Log, TEXT("DistanceVertex(%d)"), i);
  //          
  //      }
  //      //4 : ����
  //      //5 : ����
  //      //6 : �ϴ�
  //      //7 : ���
  //  }
  //  UE_LOG(LogTemp, Log, TEXT("VertexArray(%d)"), j);
  //  DrawDebugSphere(TargetActor->GetWorld(), VertexArray[5], 10.0f, 12, FColor::Red, false, 5.0f);
  //  for (int i = 0; i<NewVertexArray.Num(); i++)
  //  {
		//UE_LOG(LogTemp, Log, TEXT("NewVertexArray(%s)"), *NewVertexArray[i].ToString());
		//DrawDebugSphere(TargetActor->GetWorld(), NewVertexArray[i], 10.0f, 12, FColor::Yellow, false, 5.0f);

  //  }
  //  UE_LOG(LogTemp, Log, TEXT("Index(%d)"), NewVertexArray.Num());
    

    return NewVertexArray;
}
