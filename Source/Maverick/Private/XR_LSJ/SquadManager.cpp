// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/SquadManager.h"
#include "XR_LSJ/AISquad.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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
void ASquadManager::TestMove()
{
	//FHitResult OutHit;
	//FVector Start = ArrivalPoint + FVector(0, 0, 100);
	//FVector End = ArrivalPoint + FVector(0, 0, -100);
	//ETraceTypeQuery TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
	//FCollisionQueryParams Params;
	//TArray<AActor*> ActorsToIgnore;
	//ActorsToIgnore.Add(this);
	//if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, End, FVector(500, 500, 10), GetActorRotation(), TraceChannel, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true))
	//{
	//	FBox Bounds = OutHit.GetActor()->GetComponentsBoundingBox();
	//	TestPoint = GetBoundingBoxPointsSortedByDistance(SquadArray[0], OutHit.GetActor(), 100.0f);
	//	for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
	//	{
	//		SquadArray[SquadCount]->FSMComp->SetArrivalPoint(TestPoint[SquadCount]);
	//		SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
	//	}
	//}

    FHitResult OutHit;
	FVector Start = ArrivalPoint + FVector(0,0,100);
	FVector End = ArrivalPoint + FVector(0,0,-100);
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility;
	FCollisionQueryParams Params;

	//�������� ���� ������ �׵θ��� �̵�
	if (GetWorld()->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		TraceChannel,
		Params))
	{

		FBox Bounds = OutHit.GetActor()->GetComponentsBoundingBox();
		TestPoint = GetBoundingBoxPointsSortedByDistance(SquadArray[0], OutHit.GetActor(), 100.0f);
		for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
		{
			SquadArray[SquadCount]->FSMComp->SetArrivalPoint(TestPoint[SquadCount]);
			SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
		}
	}
	else
		for (int SquadCount = 0; SquadCount < SquadArray.Num(); SquadCount++)
		{
			SquadArray[SquadCount]->FSMComp->SetArrivalPoint(ArrivalPoint + SquadPositionArray[SquadCount]);
			SquadArray[SquadCount]->FSMComp->SetState(EEnemyState::MOVE);
		}
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

    if (!TargetActor)
    {
        return Points;
    }

    // ������ �ٿ�� �ڽ� ���
    FBox Bounds = TargetActor->GetComponentsBoundingBox();
    FVector Min = Bounds.Min;
    FVector Max = Bounds.Max;

    // X, Y, Z ���� 50cm �������� �̵��ϸ� ���� ����
    for (float X = Min.X; X <= Max.X; X += Interval)
    {
        for (float Y = Min.Y; Y <= Max.Y; Y += Interval)
        {
		    // ��迡 �ش��ϴ� ���� �߰� (��, �ڽ��� ǥ�鿡 �ִ� ���鸸)
            if (X == Min.X || X == Max.X || Y == Min.Y || Y == Max.Y)
            {
                Points.Add(FVector(X, Y, SquadArray[0]->GetActorLocation().Z));
            }
        }
    }

    return Points;
}