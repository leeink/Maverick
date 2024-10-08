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
//���� ��ġ���� ������ ��ġ�� ������ �Ʒ�����
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

    bool UpDirection = false;

    // ��� ó��
    if (bHit)
    {
        
        FBox Bounds = HitResult.GetActor()->GetComponentsBoundingBox();
        //SquadArray[0]�� �ִ°� �ƴ϶� �ٸ� ����� ã�ƺ���
        //SquadArray�� Ž���ؼ� ������ ����
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
	
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 �� ������)
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
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 ���� �� �ʷϻ�)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
    //LineTrace�� �浹�� ��ü�� �ִ��� Ȯ���ؼ� �� ��ü�� ��谪�� ���� 
 //   FHitResult OutHit;
	//FVector Start = ArrivalPoint + FVector(0,0,100);
	//FVector End = ArrivalPoint + FVector(0,0,-100);
	//ECollisionChannel TraceChannel = ECollisionChannel::ECC_GameTraceChannel1; //PawnMove //Block ���� �ؾ� ��
	//FCollisionQueryParams Params;

	////�������� ���� ������ �׵θ��� �̵�
	//if (GetWorld()->LineTraceSingleByChannel(
	//	OutHit,
	//	Start,
	//	End,
	//	TraceChannel,
	//	Params))
	//{

	//	FBox Bounds = OutHit.GetActor()->GetComponentsBoundingBox();
 //       //SquadArray[0]�� �ִ°� �ƴ϶� �ٸ� ����� ã�ƺ���
 //       //SquadArray�� Ž���ؼ� ������ ����
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

// ���� ����(50cm)���� �ٿ�� �ڽ� ������ �ѷ��� ������ MyActor���� �Ÿ� �������� ����
TArray<FVector> ASquadManager::GetBoundingBoxPointsSortedByDistance(AActor* MyActor, AActor* TargetActor, float Interval /*= 50.0f*/)
{
    TArray<FVector> Points;
    TArray<float> PointLength;

    if (!TargetActor)
    {
        return Points;
    }

    // ������ �ٿ�� �ڽ� ���
    //FBox Bounds = TargetActor->GetComponentsBoundingBox();
    //FVector Min = Bounds.Min;
    //FVector Max = Bounds.Max;
    FBox Bounds;
    FVector Min;
    FVector Max;

    TSet<UActorComponent*> ComponentSet = TargetActor->GetComponents();
    TArray<UActorComponent*> ComponentArray;
    ComponentArray.Append(ComponentSet.Array()); 

    //�浹�� ������ FTransform ������ ���� ������Ʈ�� ��������
	for (UActorComponent* Component : ComponentArray)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Component))
		{
			if (PrimitiveComp->IsRegistered() && PrimitiveComp->IsCollisionEnabled())
			{
                FTransform ComponentTransform = PrimitiveComp->GetComponentTransform();
                FBox ComponentBounds = PrimitiveComp->Bounds.GetBox();

                // ��ȿ�� �ٿ�� �ڽ��� �ִ��� Ȯ���ϰ� �ٿ�� �ڽ��� Min, Max �� ���ϱ�
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
                PointLength.Add(FVector::Distance(BoundaryPoint,MyActor->GetActorLocation()));
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