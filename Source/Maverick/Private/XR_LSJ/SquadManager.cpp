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
	   // Ʈ���̽� ���۰� �� ��ġ ����
    FVector BoxStart = ArrivalPoint;
    FVector BoxEnd = BoxStart; // X������ 1000 ���� ������ ��

    // �ڽ� ũ�� ���� (X, Y, Z ������)
    FVector BoxHalfSize = FVector(500.f, 500.f, 100.f);

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
    if (bHit)
    {
        // �浹�� ���� �̸� ���
        if (HitResult.GetActor())
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
        }

        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 �� ������)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else
    {
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 ���� �� �ʷϻ�)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }

    FHitResult OutHit;
	FVector Start = ArrivalPoint + FVector(0,0,100);
	FVector End = ArrivalPoint + FVector(0,0,-100);
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_GameTraceChannel1; //PawnMove //Block ���� �ؾ� ��
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