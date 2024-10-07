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
	   // 트레이스 시작과 끝 위치 설정
    FVector BoxStart = ArrivalPoint;
    FVector BoxEnd = BoxStart; // X축으로 1000 유닛 떨어진 곳

    // 박스 크기 설정 (X, Y, Z 반지름)
    FVector BoxHalfSize = FVector(500.f, 500.f, 100.f);

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
    if (bHit)
    {
        // 충돌한 액터 이름 출력
        if (HitResult.GetActor())
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
        }

        // 디버그용 박스 트레이스 시각화 (충돌 시 빨간색)
        DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else
    {
        // 디버그용 박스 트레이스 시각화 (충돌 없을 시 초록색)
        DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }

    FHitResult OutHit;
	FVector Start = ArrivalPoint + FVector(0,0,100);
	FVector End = ArrivalPoint + FVector(0,0,-100);
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_GameTraceChannel1; //PawnMove //Block 으로 해야 함
	FCollisionQueryParams Params;

	//목적지를 가린 액터의 테두리로 이동
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

// 일정 간격(50cm)으로 바운딩 박스 주위를 둘러싼 점들을 MyActor와의 거리 기준으로 정렬
TArray<FVector> ASquadManager::GetBoundingBoxPointsSortedByDistance(AActor* MyActor, AActor* TargetActor, float Interval /*= 50.0f*/)
{
    TArray<FVector> Points;

    if (!TargetActor)
    {
        return Points;
    }

    // 액터의 바운딩 박스 계산
    FBox Bounds = TargetActor->GetComponentsBoundingBox();
    FVector Min = Bounds.Min;
    FVector Max = Bounds.Max;

    // X, Y, Z 축을 50cm 간격으로 이동하며 점을 생성
    for (float X = Min.X; X <= Max.X; X += Interval)
    {
        for (float Y = Min.Y; Y <= Max.Y; Y += Interval)
        {
		    // 경계에 해당하는 점만 추가 (즉, 박스의 표면에 있는 점들만)
            if (X == Min.X || X == Max.X || Y == Min.Y || Y == Max.Y)
            {
                Points.Add(FVector(X, Y, SquadArray[0]->GetActorLocation().Z));
            }
        }
    }

    return Points;
}