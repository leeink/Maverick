// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/EnemyManager.h"
#include "XR_LSJ/EnemyCount.h"
#include "XR_LSJ/EnemySpawnPoint.h"
#include "XR_LSJ/SquadManager.h"
#include "Kismet/GameplayStatics.h"
#include "XR_LSJ/AITankPawn.h"
#include "XR_LSJ/GameResultWidget.h"
#include "LDG/Soldier.h"
#include "UserControlUI.h"
#include "LDG/TankBase.h"

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

	if (SoldierCount <= 0 && TankCount <= 0)
	{
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
void AEnemyManager::DiePlayerSoldier()
{
	PlayerSoldierCount--;
	UserControlUI->SetSoldierCount(PlayerSoldierCount);
	UE_LOG(LogTemp,Error,TEXT("DiePlayerSoldier"));
	if (PlayerSoldierCount <= 0 && PlayerTankCount <= 0)
	{
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
void AEnemyManager::DiePlayerTank()
{
	PlayerTankCount--;
	UserControlUI->SetTankCount(PlayerTankCount);

	if (PlayerSoldierCount <= 0 && PlayerTankCount <= 0)
	{
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
void AEnemyManager::ShowResult()
{
	UGameplayStatics::SetGamePaused(GetWorld(),true);

	if (GameResultClass)
	{
		GameResultWidget = Cast<UGameResultWidget>(CreateWidget(GetWorld(),GameResultClass));
		GameResultWidget->AddBasicSlot();
		FString NickNameAI = "AI";
		FString NickNamePlayer = "Player";
		GameResultWidget->AddNorthKoreaData(NickNameAI, MaxPlayerSoldierCount-PlayerSoldierCount, MaxSoldierCount-SoldierCount, MaxPlayerTankCount-PlayerTankCount, MaxTankCount-TankCount);
		GameResultWidget->AddSouthKoreaData(NickNamePlayer, MaxSoldierCount-SoldierCount, MaxPlayerSoldierCount-PlayerSoldierCount, MaxTankCount-TankCount, MaxPlayerTankCount-PlayerTankCount);
		FString Time = "30:00";
		GameResultWidget->SetClearTime(Time);
		GameResultWidget->AddToViewport();
	}
}

void AEnemyManager::DieTank()
{
	TankCount--;
	EnemyCountWidget->SetTankCount(TankCount);

	if (SoldierCount <= 0 && TankCount <= 0)
	{
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyCountClass)
	{
		EnemyCountWidget = Cast<UEnemyCount>(CreateWidget(GetWorld(),EnemyCountClass));
		EnemyCountWidget->AddToViewport();
	}

	//적 스폰
	//월드에 있는 특정 박스 오브젝트를 찾고 그 위치에 스폰
	//count ++ 해줌
	//Vector 에 목록 저장
	//soldier 죽으면 count -- , 델리게이트
	//탱크 죽으면 count -- , 델리게이트
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
			if (ASquadManager* SquadManager = GetWorld()->SpawnActor<ASquadManager>(SquadManagerClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				SquadManager->FDelSoldierUnitDie.BindUFunction(this,FName("DieSoldier"));
				SoldierCount+=SquadManager->GetCurrentSquadCount();
			}

		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::Tank)
		{
			SpawnLocation.Z = 350.0f;
			
			if (AAITankPawn* TankPawn = GetWorld()->SpawnActor<AAITankPawn>(TankPawnClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				TankPawn->FDelTankUnitDie.BindUFunction(this, FName("DieTank"));
				TankCount++;
			}
		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::PlayerSoldier)
		{
			SpawnLocation.Z = 208.0f;
			FActorSpawnParameters SpawnParams1;
			if (ASoldier* PlayerSoldier = GetWorld()->SpawnActor<ASoldier>(PlayerSoldierPawnClass, SpawnLocation, SpawnRotation, SpawnParams1))
			{
				PlayerSoldier->Del_PlayerSoldierUnitDie.BindUFunction(this,FName("DiePlayerSoldier"));
				PlayerSoldierCount++;
			}

		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::PlayerTank)
		{
			SpawnLocation.Z = 350.0f;
			
			if (ATankBase* TankPawn = GetWorld()->SpawnActor<ATankBase>(TankPawnClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				TankPawn->Del_PlayerTankUnitDie.BindUFunction(this, FName("DiePlayerTank"));
				PlayerTankCount++;
			}
		}
		PointActor->Destroy();
	}
	if (EnemyCountWidget)
	{
		EnemyCountWidget->SetSoldierCount(SoldierCount);
		EnemyCountWidget->SetTankCount(TankCount);
		MaxSoldierCount=SoldierCount;
		MaxTankCount=TankCount;
	}

	if (PlayerControllerClass)
	{
		UserControlUI = Cast<UUserControlUI>(CreateWidget(GetWorld(),PlayerControllerClass));
		UserControlUI->SetTankCount(PlayerTankCount);
		UserControlUI->SetSoldierCount(PlayerSoldierCount);
		UserControlUI->AddToViewport();
		MaxPlayerSoldierCount=PlayerSoldierCount;
		MaxPlayerTankCount=PlayerTankCount;
	}

	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

