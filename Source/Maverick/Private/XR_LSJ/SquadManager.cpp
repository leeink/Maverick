// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/SquadManager.h"
#include "XR_LSJ/AISquadFSMComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ActorComponent.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationPath.h"
#include "PhysicsEngine/ConvexElem.h"
#include "PhysicsEngine/BodySetup.h"
#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "XR_LSJ/AIUnitHpBar.h"
#include "Components/WidgetComponent.h"

FVector ASquadManager::GetTargetLocation()
{
    return GetActorLocation();
}

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

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    BoxComp->SetCollisionProfileName(TEXT("FindTarget"));
    SetRootComponent(BoxComp);
    
    HpWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComp"));
    HpWidgetComp->SetupAttachment(BoxComp);
    HpWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HpWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    HpWidgetComp->SetDrawSize(FVector2D(100,100));
    HpWidgetComp->SetRelativeLocation(FVector(0,0.f,400.0f));

    AIUnitCategory=EAIUnitCategory::SQUAD;
}

EAIUnitCommandState ASquadManager::GetCurrentCommandState()
{
    return CurrentCommandState;
}

void ASquadManager::SetCommandState(EAIUnitCommandState Command)
{
}

// Called when the game starts or when spawned
void ASquadManager::BeginPlay()
{
	Super::BeginPlay();
   
    SquadManagerAbility.Hp = 100.f;
    
    
    SquadManagerAbility.FindTargetRange = 2000.f;
	for (int SpawnCount = 0; SpawnCount < MaxSpawnCount; SpawnCount++)
	{
		SquadArray.Add(GetWorld()->SpawnActor<AAISquad>(SpawnSquadPactory, GetActorLocation() + SquadPositionArray[SpawnCount], GetActorRotation()));
		SquadArray[SpawnCount]->SetMySquadNumber(SpawnCount);
        SquadArray[SpawnCount]->SetSquadAbility(SquadManagerAbility);
        SquadArray[SpawnCount]->FDelTargetDie.BindUFunction(this, FName("FindCloseTargetUnit"));
        SquadArray[SpawnCount]->FDelSquadUnitDamaged.BindUFunction(this, FName("DamagedSquadUnit"));
        SquadArray[SpawnCount]->FDelSquadUnitDie.BindUFunction(this, FName("DieSquadUnit"));
        MaxSquadHp += SquadManagerAbility.Hp;
        CurrentSquadHp = MaxSquadHp;
	}
    UE_LOG(LogTemp, Warning, TEXT("MaxSquadHp %d"),MaxSquadHp);
    CurrentAttachedSquadNumber = 0;
    AttachToComponent(GetSquadArray()[CurrentAttachedSquadNumber]->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale);
    //AttachToActor(SquadArray[0],FAttachmentTransformRules::SnapToTargetIncludingScale);
	FTimerHandle handle;
	//GetWorld()->GetTimerManager().SetTimer(handle, this, &ASquadManager::CheckLocationForObject, 10.0f, true);
    FTimerHandle FindEnemy;
	GetWorld()->GetTimerManager().SetTimer(FindEnemy, this, &ASquadManager::FindCloseTargetUnit, 10.0f, true);
    
    //HpBar
    if (HpWidgetComp && HpBarClass)
    {
        HpWidgetComp->SetWidgetClass(HpBarClass);
        UAIUnitHpBar* HpBarUI =Cast<UAIUnitHpBar>(HpWidgetComp->GetUserWidgetObject());
		if (HpBarUI)
		    HpBarUI->SetUISquadImage();
    }
        
    SetCurrentSquadCount(MaxSpawnCount);
}
// ȿ������ �Ÿ� �� �Լ� (������ ���� ����)
bool ASquadManager::IsCloserThan(const FVector& PointA, const FVector& PointB, const FVector& PointC)
{
    float DistSquaredOrign = (PointB-PointA).SizeSquared();
    float DistSquaredNew = (PointC-PointA).SizeSquared();

    return DistSquaredNew < DistSquaredOrign;
}
//����� �� Ž��
void ASquadManager::FindCloseTargetUnit()
{
    FVector Start = GetActorLocation();
    FVector End = GetActorLocation();
    float Radius = SquadManagerAbility.FindTargetRange;
    ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);
    bool bTraceComplex = false;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
	for (AActor* SquadActor : GetSquadArray())
		ActorsToIgnore.Add(SquadActor);
    EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::ForOneFrame;
    TArray<FHitResult> OutHits;
    bool bIgnoreSelf = true;
    FLinearColor TraceColor = FLinearColor::Gray;
    FLinearColor TraceHitColor = FLinearColor::Blue;
    float DrawTime = 1.0f;
    //SquadManagerAbility.FindTargetRange ���� Ž��
    const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,Radius,TraceChannel,bTraceComplex,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf,TraceColor,TraceHitColor);
    if (Hit)
    {
        Target.Empty();
        Target.Init(OutHits.Last().GetActor(),MaxSpawnCount);
        
        bool CanAttackEnemy = false;
        int SquadLastIdx = 0;
        for (int SquadCount = 0; SquadCount<MaxSpawnCount; SquadCount++)
		{
            if(SquadArray[SquadCount]==nullptr)
                continue;
            if(SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
               continue;
            SquadLastIdx = SquadCount;
        }
        
        for (const FHitResult& HitResult : OutHits)
        {
            IIAICommand* FoundEnemy = Cast<IIAICommand>(HitResult.GetActor());
            if(nullptr == FoundEnemy)
                continue;
            if(FoundEnemy && FoundEnemy->GetCurrentCommandState()==EAIUnitCommandState::DIE)
                continue;
            if(false ==HitResult.GetActor()->ActorHasTag("Enemy"))
                continue;
            
             //�� �д������ ã�� ���� �� ���� ����� ���̰� �߰��� ��ֹ��� ���ٸ� Ÿ������ ����
            //Ÿ�ٿ��� ���� ����
		    for (int SquadCount = 0; SquadCount<MaxSpawnCount; SquadCount++)
		    {
                if(SquadArray[SquadCount]==nullptr)
                    continue;
                if(SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
                    continue;
                //�߰��� ��ֹ��� ���ٸ�
                //�� �д������ �Ӹ��� �������� �� ���̿� ���ع��� ����
				FHitResult OutHit;
				FVector StartLocation = SquadArray[SquadCount]->GetMesh()->GetSocketLocation(TEXT("Head"));
				FVector EndLocation = FoundEnemy->GetTargetLocation();
				ECollisionChannel TraceChannelHit = ECC_Visibility;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(this);
				Params.AddIgnoredActor(HitResult.GetActor());
				bool CanHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannelHit, Params);
				if (CanHit)
				{
					//DrawDebugLine(GetWorld(), StartLocation, FoundEnemy->GetTargetLocation() , FColor::Blue,false,10.0f);
				}
				else
				{
                    CanAttackEnemy = true;
                    if (IsCloserThan(StartLocation, Target[SquadCount]->GetActorLocation(), FoundEnemy->GetTargetLocation()))
                    {
                        Target[SquadCount] = HitResult.GetActor();
                        //DrawDebugLine(GetWorld(), StartLocation, FoundEnemy->GetTargetLocation() , FColor::Red,false,10.0f);
                    }
				}
		    }

            if(Target[SquadLastIdx] != OutHits.Last().GetActor())
                break;
            
        }
		//������ ���� ���� ������� �����ϹǷ� �ѹ� �� ���̿� ��ֹ��� �ִ��� �˻��ؾ��Ѵ�.
		for (int SquadCount = 0; SquadCount < MaxSpawnCount; SquadCount++)
		{
            if(SquadArray[SquadCount]==nullptr)
                continue;
            if (SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
				continue;
            if (Target[SquadCount] == OutHits.Last().GetActor())
            {
                IIAICommand* FoundEnemy = Cast<IIAICommand>(OutHits.Last().GetActor());
                FHitResult OutHit;
				FVector StartLocation = SquadArray[SquadCount]->GetMesh()->GetSocketLocation(TEXT("Head"));
				FVector EndLocation = FoundEnemy->GetTargetLocation();
				ECollisionChannel TraceChannelHit = ECC_Visibility;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(this);
				Params.AddIgnoredActor(OutHits.Last().GetActor());
				bool CanHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannelHit, Params);
				if (CanHit)
				{
					Target[SquadCount] = nullptr;
				}
            }
		}
        //Ž�� ���� �ȿ� ���� �����Ѵ�.
        if (CanAttackEnemy)
            AttackTargetUnit();
        else //Ž�� ���� �ȿ� �� ���������� ���ع��� �ִٸ� ������ �ߴ��Ѵ�.
        { 
            for (int SquadCount = 0; SquadCount<MaxSpawnCount; SquadCount++)
			{
                if(SquadArray[SquadCount]==nullptr)
                    continue;
				if (SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
					continue;
				if (SquadArray[SquadCount]->FSMComp->GetIsAttacking())
					SquadArray[SquadCount]->FSMComp->SetIsAttacking(false, nullptr);
			}
        }
    }
    else //Ž�� ���� �ȿ��� ���� ã�� �� ���ٸ�
    {
         for (int SquadCount = 0; SquadCount<MaxSpawnCount; SquadCount++)
		 {
            if(SquadArray[SquadCount]==nullptr)
                continue;
			if(SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
				 continue;
            if(SquadArray[SquadCount]->FSMComp->GetIsAttacking())
                SquadArray[SquadCount]->FSMComp->SetIsAttacking(false,nullptr);
		 }
    }
}
void ASquadManager::FindTargetSquad()
{
    FVector Start = GetActorLocation();
    FVector End = GetActorLocation();
    float Radius = SquadManagerAbility.FindTargetRange;
    ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3);
    bool bTraceComplex = false;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    for(AActor* SquadActor : GetSquadArray())
        ActorsToIgnore.Add(SquadActor);
    EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::ForOneFrame;
    TArray<FHitResult> OutHits;
    bool bIgnoreSelf = true;
    FLinearColor TraceColor = FLinearColor::Gray;
    FLinearColor TraceHitColor = FLinearColor::Blue;
    float DrawTime = 1.0f;

    const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,Radius,TraceChannel,bTraceComplex,ActorsToIgnore,DrawDebugType,OutHits,bIgnoreSelf,TraceColor,TraceHitColor);
    if (Hit)
    {
        Target.Empty();
        for (const FHitResult& HitResult : OutHits)
        {
            if (HitResult.GetActor()->ActorHasTag(TEXT("SquadManager")))
            {
                FHitResult OutHit;
				FVector StartLocation = GetActorLocation();
				FVector EndLocation = HitResult.GetActor()->GetActorLocation();
				ECollisionChannel TraceChannelHit = ECC_Visibility;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(this);
             	Params.AddIgnoredActor(HitResult.GetActor());
				bool CanHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannelHit, Params);
				if (CanHit)
				{
                    UE_LOG(LogTemp, Warning, TEXT("%s"),*HitResult.GetActor()->GetName());
                    //DrawDebugLine(GetWorld(), StartLocation, EndLocation , FColor::Red,false,10.0f);
				}
                else
                {
                    Target.Add(HitResult.GetActor());
                }
                //DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue);
            }
        }
    }
    if (Target.IsEmpty() == false)
    {
        
            FTimerHandle AttackEnemy;
			FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ASquadManager::AttackTargetSquad, Target[0]);
			GetWorldTimerManager().SetTimer(AttackEnemy, RespawnDelegate, 10.0f, true);
    }
}
void ASquadManager::AttackTargetUnit()
{
    if(Target.Num() < CurrentSquadCount)
       return;
     for (int SquadCount = 0; SquadCount<MaxSpawnCount; SquadCount++)
    {
        if(SquadArray[SquadCount]==nullptr)
            continue;
        if(SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
            continue;
		if (Target[SquadCount] == nullptr)
			continue;
        //UE_LOG(LogTemp, Warning, TEXT("Owner %s Target %s"),*SquadArray[SquadCount]->GetName(),*Target[SquadCount]->GetName() );
        SquadArray[SquadCount]->FSMComp->SetIsAttacking(true, Target[SquadCount]);
    }
}
void ASquadManager::AttackTargetSquad(AActor* TargetActor)
{
    if(nullptr == TargetActor)
        return;
    ASquadManager* TargetSquadManager =Cast<ASquadManager>(TargetActor);
    for (AAISquad* SquadPawn : GetSquadArray())
    {
        if(SquadPawn==nullptr)
            continue;
        if(SquadPawn->FSMComp->GetCurrentState() == EEnemyState::DIE)
            continue;
        //�����Ÿ� �ȿ� �ְ� //���鿡�� ������
        //�ӽ÷� ����
        int RandomIdx = FMath::RandRange(0, TargetSquadManager->GetCurrentSquadCount()-1);
        SquadPawn->FSMComp->SetIsAttacking(true, TargetSquadManager->GetSquadArray()[RandomIdx]);
    }
}
void ASquadManager::FindPath(const FVector& TargetLocation)
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if(nullptr == NavSystem)
        return;
    FVector StartLocation = GetSquadArray()[0]->GetActorLocation();
    UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation);
    if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
        TArray<FVector> ArrayLocation;
        ArrayLocation = NavPath->PathPoints;
       for (int SquadCount = 0; SquadCount < GetSquadArray().Num(); SquadCount++)
	   {
           if(SquadArray[SquadCount]==nullptr)
            continue;
           if(SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
            continue;
           FVector DirectionPosition = GetActorForwardVector()*SquadPositionArray[SquadCount].X+GetActorRightVector()*SquadPositionArray[SquadCount].Y;
           DirectionPosition.Z = 0;
           ArrayLocation.Last()+=DirectionPosition;
           GetSquadArray()[SquadCount]->FSMComp->SetSquadPosition(DirectionPosition);
		   GetSquadArray()[SquadCount]->FSMComp->MovePathAsync(ArrayLocation);
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
    FVector StartLocation = GetSquadArray()[0]->GetActorLocation();
   UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(),StartLocation,TargetLocation[0]);
    
    if (NavPath && NavPath->IsValid() && !NavPath->IsPartial())
    {
        TArray<FVector> ArrayLocation;
        ArrayLocation = NavPath->PathPoints;
       for (int SquadCount = 0; SquadCount < GetSquadArray().Num(); SquadCount++)
	   {
           if(SquadArray[SquadCount]==nullptr)
            continue;
           if(SquadArray[SquadCount]->FSMComp->GetCurrentState() == EEnemyState::DIE)
            continue;
           ArrayLocation.Last() = TargetLocation[SquadCount];
           FVector DirectionPosition = GetActorForwardVector()*SquadPositionArray[SquadCount].X+GetActorRightVector()*SquadPositionArray[SquadCount].Y;
           DirectionPosition.Z = 0;
           GetSquadArray()[SquadCount]->FSMComp->SetSquadPosition(DirectionPosition);
		   GetSquadArray()[SquadCount]->FSMComp->MovePathAsync(ArrayLocation);
	   }
    }
    else //if(NavPath && NavPath->IsValid() && NavPath->IsPartial()) // ��ΰ� ��������
    {
         //UE_LOG(LogTemp, Warning, TEXT("Reached final destination!"));
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
        //UE_LOG(LogTemp, Log, TEXT("ObstructionPoints(%d)"), ObstructionPoints.Num());
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
        //DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
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
        //UE_LOG(LogTemp, Log, TEXT("ObstructionPoints(%d)"), ObstructionPoints.Num());
        FindObstructionPath(ObstructionPoints);
        ArrivalPoint *= -1;
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 �� ������)
        //DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxHalfSize, Rotation, FColor::Red, false, 2.f);
    }
    else //��ǥ������ ������Ʈ�� ���ٸ�
    {
       FindPath(TargetLocation);
        // ����׿� �ڽ� Ʈ���̽� �ð�ȭ (�浹 ���� �� �ʷϻ�)
        //DrawDebugBox(GetWorld(), BoxEnd, BoxHalfSize, Rotation, FColor::Green, false, 2.f);
    }
}

void ASquadManager::DamagedSquadUnit(float Damage)
{
    UAIUnitHpBar* HpBarUI =Cast<UAIUnitHpBar>(HpWidgetComp->GetUserWidgetObject());
    if (HpBarUI)
    {
        HpBarUI->SetHpBar((float)(CurrentSquadHp-=Damage)/MaxSquadHp);
        UE_LOG(LogTemp, Warning, TEXT("CurrentSquadHp %d"),CurrentSquadHp);
        if (CurrentSquadHp <= 0)
        {
            HpBarUI->SetVisibility(ESlateVisibility::Collapsed);
            HpWidgetComp->Deactivate();
        }
    }
}

void ASquadManager::DieSquadUnit(int32 SquadNumber)
{
    CurrentSquadCount--;
    		SquadArray[SquadNumber]->FDelTargetDie.Unbind();
			SquadArray[SquadNumber]->FDelSquadUnitDamaged.Unbind();
			SquadArray[SquadNumber]->FDelSquadUnitDie.Unbind();
    if (CurrentAttachedSquadNumber == SquadNumber)
    {
		for (int SpawnCount = 0; SpawnCount < MaxSpawnCount; SpawnCount++)
		{
            if(SquadArray[SpawnCount]==nullptr)
                continue;
            if (SquadArray[SpawnCount]->FSMComp->GetCurrentState() != EEnemyState::DIE)
            {
                AttachToComponent(SquadArray[SpawnCount]->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale);
                break;
            }
        }
    }
    FTimerHandle DestroyUnitHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyUnitHandle, [&]()
		{
			SquadArray[SquadNumber]->Destroy();
            SquadArray[SquadNumber]=nullptr;
            UE_LOG(LogTemp, Log, TEXT("SquadNumber (%d)"), SquadNumber);
            UE_LOG(LogTemp, Log, TEXT("CurrentSquadCount (%d)"), CurrentSquadCount);
		}, 2.0f, false);
}

// Called every frame
void ASquadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// �д�� ����ŭ FVector �����
void ASquadManager::MakeObstructionPoint(AActor* TargetActor, TArray<FVector>& OutPoints, EObstructionDirection DirectionNum)
{
    if (OutPoints.Num() == 1 || (OutPoints.Num()<GetCurrentSquadCount() && OutPoints.Num() >= 1)) //�ӽ� ��ġ�� �д������ OutPoints.Num()�� ������ ��ġ�� �߰��Ѵ�.
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
        //DrawDebugSphere(TargetActor->GetWorld(), OutPoints[0], 10.0f, 12, FColor::Black, false, 5.0f);
        for (int SquadCount = 1; SquadCount < GetSquadArray().Num(); SquadCount++)
	    {
			FVector DirectionPosition = BaseForwardVector * SquadPositionArray[SquadCount].X + BaseRightVector * SquadPositionArray[SquadCount].Y;
			DirectionPosition.Z = 0;
			OutPoints[SquadCount]=(OutPoints[0]+DirectionPosition);// += SquadPositionArray[SquadCount]);
            //UE_LOG(LogTemp, Log, TEXT("NewVertexArray(%s)"), *OutPoints[SquadCount].ToString());
		    //DrawDebugSphere(TargetActor->GetWorld(), OutPoints[SquadCount], 10.0f, 12, FColor::Blue, false, 5.0f);
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
        Point.Z = GetSquadArray()[0]->GetActorLocation().Z;
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
         EObstructionDirection ObstructionDirection = EObstructionDirection::Left;
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
		 if (NewVertexArray.Num() < GetCurrentSquadCount())
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
