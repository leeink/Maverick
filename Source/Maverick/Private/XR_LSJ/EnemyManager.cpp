// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/EnemyManager.h"
#include "XR_LSJ/EnemyCount.h"
#include "XR_LSJ/EnemySpawnPoint.h"
#include "XR_LSJ/SquadManager.h"
#include "Kismet/GameplayStatics.h"
#include "XR_LSJ/AITankPawn.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SoldierCount=0;
	TankCount=0;
}
void AEnemyManager::DieSoldier()
{
	SoldierCount--;
	EnemyCountWidget->SetSoldierCount(SoldierCount);
}
void AEnemyManager::DieTank()
{
	UE_LOG(LogTemp,Error,TEXT("TankCount"));
	TankCount--;
	EnemyCountWidget->SetTankCount(TankCount);
}
// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyCountClass)
	{
		EnemyCountWidget = Cast<UEnemyCount>(CreateWidget(GetWorld(),EnemyCountClass));
		EnemyCountWidget->AddToViewport();
		EnemyCountWidget->SetTankCount(2);
		EnemyCountWidget->SetSoldierCount(5*6);
	}

	//�� ����
	//���忡 �ִ� Ư�� �ڽ� ������Ʈ�� ã�� �� ��ġ�� ����
	//count ++ ����
	//Vector �� ��� ����
	//soldier ������ count -- , ��������Ʈ
	//��ũ ������ count -- , ��������Ʈ
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPoint::StaticClass(), FoundActors);
	for (AActor* PointActor : FoundActors)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		FVector SpawnLocation = PointActor->GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		AEnemySpawnPoint* EnemySpawnPoint = Cast<AEnemySpawnPoint>(PointActor);
		if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::Soldier)
		{
			SpawnLocation.Z = 190.0f;
			ASquadManager* SquadManager = GetWorld()->SpawnActor<ASquadManager>(SquadManagerClass, SpawnLocation, SpawnRotation, SpawnParams);
			SquadManager->FDelSoldierUnitDie.BindUFunction(this,FName("DieSoldier"));
			SoldierCount+=SquadManager->GetCurrentSquadCount();
		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::Tank)
		{
			SpawnLocation.Z = 350.0f;
			AAITankPawn* TankPawn = GetWorld()->SpawnActor<AAITankPawn>(TankPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
			TankPawn->FDelTankUnitDie.BindUFunction(this,FName("DieTank"));
			TankCount++;
		}

		PointActor->Destroy();
	}

	EnemyCountWidget->SetSoldierCount(SoldierCount);
	EnemyCountWidget->SetTankCount(TankCount);


	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

