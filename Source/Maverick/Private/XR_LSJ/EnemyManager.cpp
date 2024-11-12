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
#include "XR_LSJ/OccupiedLocation.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
void AEnemyManager::DieSoldier()
{
	if(EndGame)
		return;
	SoldierCount--;
	EnemyCountWidget->SetSoldierCount(SoldierCount);

	if (SoldierCount <= 0 && TankCount <= 0)
	{
		EndGame = true;
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
void AEnemyManager::DiePlayerSoldier()
{
	if(EndGame)
		return;
	PlayerSoldierCount--;
	UserControlUI->SetSoldierCount(PlayerSoldierCount);

	if (PlayerSoldierCount <= 0 && PlayerTankCount <= 0)
	{
		EndGame = true;
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
void AEnemyManager::DiePlayerTank()
{
	if(EndGame)
		return;

	PlayerTankCount--;
	UserControlUI->SetTankCount(PlayerTankCount);

	if (PlayerSoldierCount <= 0 && PlayerTankCount <= 0)
	{
		EndGame = true;
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
FString AEnemyManager::ConvertSecondsToMinutesAndSeconds(int32 TotalSeconds)
{
    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;
    
    // �� �ڸ� ������ �����Ϸ��� �߰� ����
    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
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
		
		FString Time = ConvertSecondsToMinutesAndSeconds(PlayTimeSeconds);
		GameResultWidget->SetClearTime(Time);
		GameResultWidget->AddToViewport();
	}
}

void AEnemyManager::DieTank()
{
	if(EndGame)
		return;
	TankCount--;
	EnemyCountWidget->SetTankCount(TankCount);

	if (SoldierCount <= 0 && TankCount <= 0)
	{
		EndGame = true;
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,this,&AEnemyManager::ShowResult,3.0f,false);
	}
}
// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	OccupiedTop = true;
	OccupiedBottom = false;
	EndGame = false;
	if (EnemyCountClass)
	{
		EnemyCountWidget = Cast<UEnemyCount>(CreateWidget(GetWorld(),EnemyCountClass));
		EnemyCountWidget->AddToViewport();
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
			if (ASquadManager* SquadManager = GetWorld()->SpawnActor<ASquadManager>(SquadManagerClass, SpawnLocation, SpawnRotation, SpawnParams))
			{	
				SquadManager->SetDefenseMode(false);
				SquadManager->FDelSoldierUnitDie.BindUFunction(this,FName("DieSoldier"));		
				SquadManager->SetMinimapUIZOrder(SoldierCount);
				if(!EnemySpawnPoint->GetStartLocation().Equals(FVector::ZeroVector))
					SquadManager->CheckLocationForObject(EnemySpawnPoint->GetStartLocation());
				SoldierCount+=SquadManager->GetCurrentSquadCount();
				EnemySquadAll.Add(SquadManager);

			}

		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::Tank)
		{
			SpawnLocation.Z = 350.0f;
			if (AAITankPawn* TankPawn = GetWorld()->SpawnActor<AAITankPawn>(TankPawnClass, SpawnLocation, SpawnRotation, SpawnParams))
			{	
				TankPawn->SetDefenseMode(false);
				TankPawn->FDelTankUnitDie.BindUFunction(this, FName("DieTank"));
				TankPawn->SetMinimapUIZOrder(SoldierCount+TankCount);
				if(!EnemySpawnPoint->GetStartLocation().Equals(FVector::ZeroVector))
					TankPawn->FindPath(EnemySpawnPoint->GetStartLocation());
				EnemyTankAll.Add(TankPawn);
				TankCount++;
			}
		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::PlayerSoldier)
		{
			SpawnLocation.Z = 208.0f;
			FActorSpawnParameters SpawnParams1;
			if (ASoldier* PlayerSoldier = GetWorld()->SpawnActor<ASoldier>(PlayerSoldierPawnClass, SpawnLocation, SpawnRotation, SpawnParams1))
			{
				PlayerSquadAll.Add(PlayerSoldier);
				PlayerSoldier->Del_PlayerSoldierUnitDie.BindUFunction(this,FName("DiePlayerSoldier"));
				PlayerSoldierCount++;
			}

		}
		else if (EnemySpawnPoint&&EnemySpawnPoint->GetMOS() == EMOS::PlayerTank)
		{
			SpawnLocation.Z = 120.f;
			
			if (ATankBase* TankPawn = GetWorld()->SpawnActor<ATankBase>(PlayerTankPawnClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				PlayerTankAll.Add(TankPawn);
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

	TopOccupiedLocationStruct.DefensiveUnit.Init(false,2);
	BottomOccupiedLocationStruct.DefensiveUnit.Init(false,2);
	TArray<AActor*> FoundOccupiedLocationActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOccupiedLocation::StaticClass(), FoundOccupiedLocationActors);
	for (AActor* PointActor : FoundOccupiedLocationActors)
	{
		AOccupiedLocation* OccupiedLocationActor = Cast<AOccupiedLocation>(PointActor);
		if (OccupiedLocationActor)
		{
			if (OccupiedLocationActor->GetIsTop())
				UpdateOccupiedLocationStruct(TopOccupiedLocationStruct,OccupiedLocationActor);
			else
				UpdateOccupiedLocationStruct(BottomOccupiedLocationStruct,OccupiedLocationActor);
		}
	}
	FTimerHandle BeginCheckPlayerUnitLocationHandle;
	GetWorld()->GetTimerManager().SetTimer(BeginCheckPlayerUnitLocationHandle,this,&AEnemyManager::CheckPlayerUnitLocation,5.0f,false);
	GetWorld()->GetTimerManager().SetTimer(CheckPlayerUnitLocationHandle,this,&AEnemyManager::CheckPlayerUnitLocation,50.0f,true);
}
void AEnemyManager::UpdateOccupiedLocationStruct(FOccupiedLocationStruct& pOccupiedLocationStruct,AOccupiedLocation* pOccupiedLocationActor)
{
	switch (pOccupiedLocationActor->GetOccupiedLocationType())
	{
	case EOccupiedLocationType::Occupation:
		pOccupiedLocationStruct.OccupiedLocation = pOccupiedLocationActor->GetActorLocation();
		break;
	case EOccupiedLocationType::Tank:
		pOccupiedLocationStruct.TankLocation.Add(pOccupiedLocationActor->GetActorLocation());
		break;
	case EOccupiedLocationType::Squad:
		FVector Location = pOccupiedLocationActor->GetActorLocation();
		Location.Z = 190.f;
		pOccupiedLocationStruct.SquadLocation.Add(Location);
		break;
	}
}
void AEnemyManager::CheckPlayerUnitLocation()
{
	//�� �̵����� ������?
		//�߰��� CheckPlayerUnitLocation�� ����Ǽ�
		//LandScape�� �浹������ �Ǿ
	//������ ���� ���ٸ� ������ �����Ѵ�.
	if (false == OccupiedTop && false == OccupiedBottom)
	{
		MoveToTakeOver(TopOccupiedLocationStruct);
	}
	else if (false == OccupiedTop) //�Ʒ����� �����ߴٸ� �ּ� �д븦 �Ʒ��ʿ� ��ġ�ϰ� ������ �����Ѵ�.
	{
		//�ּ� ��� �д� ��ġ üũ
		MinDefensiveDeployment(BottomOccupiedLocationStruct);
		
		//������ �����Ѵ�.
		MoveToTakeOver(TopOccupiedLocationStruct);
	}
	else if (false == OccupiedBottom)
	{
		//�ּ� ��� �д� ��ġ üũ
		MinDefensiveDeployment(TopOccupiedLocationStruct);

		//�Ʒ����� �����Ѵ�.
		MoveToTakeOver(BottomOccupiedLocationStruct);
	}
}
//�ּ� ��� ��ġ
void AEnemyManager::MinDefensiveDeployment(FOccupiedLocationStruct& OccupiedLocationStruct)
{
		int32 UnitCount = 0;
		//����� �д븦 ��ġ ��Ų��. �д밡 ���ٸ� ��ũ�� ��ġ�Ѵ�.
		for (int32 DefensiveUnitCount = 0; DefensiveUnitCount < OccupiedLocationStruct.DefensiveUnit.Num(); DefensiveUnitCount++)
		{
			if (OccupiedLocationStruct.DefensiveUnit[DefensiveUnitCount] == false)
			{
				if (DefensiveUnitCount == 0)
				{
					float MinDistance = 100000.f;
					ASquadManager* CloseFirstSquad = nullptr;
					for (int32 pSquadManagerCount = 0; pSquadManagerCount < EnemySquadAll.Num(); pSquadManagerCount++)
					{
						if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState() == EAIUnitCommandState::DIE)
							continue;
						if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState() == EAIUnitCommandState::Defense)
							continue;
						float DefenseDistance = FVector::Distance(OccupiedLocationStruct.OccupiedLocation, EnemySquadAll[pSquadManagerCount]->GetActorLocation());
						if (MinDistance > DefenseDistance)
						{
							MinDistance = DefenseDistance;
							CloseFirstSquad = EnemySquadAll[pSquadManagerCount];
						}
					}
					if (nullptr == CloseFirstSquad)
						break;
					CloseFirstSquad->CheckLocationForObject(OccupiedLocationStruct.OccupiedLocation);
					CloseFirstSquad->SetCommandState(EAIUnitCommandState::Defense);
					OccupiedLocationStruct.DefensiveUnit[0] = true;
					UnitCount++;
				}
				else
				{
					float MinDistance = 100000.f;
					ASquadManager* CloseFirstSquad = nullptr;
					for (int32 pSquadManagerCount = 0; pSquadManagerCount < EnemySquadAll.Num(); pSquadManagerCount++)
					{
						if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState() == EAIUnitCommandState::DIE)
							continue;
						if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState() == EAIUnitCommandState::Defense)
							continue;
						float DefenseDistance = FVector::Distance(OccupiedLocationStruct.SquadLocation[0], EnemySquadAll[pSquadManagerCount]->GetActorLocation());
						if (MinDistance > DefenseDistance)
						{
							MinDistance = DefenseDistance;
							CloseFirstSquad = EnemySquadAll[pSquadManagerCount];
						}
					}
					if (nullptr == CloseFirstSquad)
						continue;
					CloseFirstSquad->CheckLocationForObject(OccupiedLocationStruct.SquadLocation[0]);
					CloseFirstSquad->SetCommandState(EAIUnitCommandState::Defense);
					OccupiedLocationStruct.DefensiveUnit[1] = true;
					UnitCount++;
				}
			}
		}
		if (UnitCount <= 0) //��ũ�� ��ġ
		{
			if (OccupiedLocationStruct.DefensiveUnit[0] == false)
			{
				float MinDistance = 100000.f;
				AAITankPawn* CloseFirstSquad = nullptr;
				for (int32 pTankCount = 0; pTankCount < EnemyTankAll.Num(); pTankCount++)
				{
					if (EnemyTankAll[pTankCount]->GetCurrentCommandState() == EAIUnitCommandState::DIE)
						continue;
					if (EnemyTankAll[pTankCount]->GetDefenseMode())
						continue;
					float DefenseDistance = FVector::Distance(OccupiedLocationStruct.OccupiedLocation, EnemyTankAll[pTankCount]->GetActorLocation());
					
					if (MinDistance > DefenseDistance)
					{
						MinDistance = DefenseDistance;
						CloseFirstSquad = EnemyTankAll[pTankCount];
					}
				}
				if (nullptr == CloseFirstSquad)
					return;
				CloseFirstSquad->SetDefenseMode(true);
				FVector Location = OccupiedLocationStruct.OccupiedLocation;
				Location.Z = 350.0f;
				CloseFirstSquad->FindPath(Location);
				OccupiedLocationStruct.DefensiveUnit[0] = true;
			}
		}
}
//���� �̵�
void AEnemyManager::MoveToTakeOver(FOccupiedLocationStruct& OccupiedLocationStruct)
{
	int32 CurrentRemainSquad = -1;
	int32 CurrentRemainTank = -1;
	for (int32 pSquadManagerCount = 0;pSquadManagerCount<EnemySquadAll.Num(); pSquadManagerCount++)
	{
		if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState()==EAIUnitCommandState::DIE)
			continue;
		if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState()==EAIUnitCommandState::Defense)
			continue;
		if (CurrentRemainSquad == -1)
		{
			EnemySquadAll[pSquadManagerCount]->CheckLocationForObject(OccupiedLocationStruct.OccupiedLocation);
		}
		else
		{
			EnemySquadAll[pSquadManagerCount]->CheckLocationForObject(OccupiedLocationStruct.SquadLocation[CurrentRemainSquad]);
		}
		CurrentRemainSquad++;
	}
	for (int32 pTankCount = 0;pTankCount<EnemyTankAll.Num(); pTankCount++)
	{
		if (EnemyTankAll[pTankCount]->GetCurrentCommandState()==EAIUnitCommandState::DIE)
			continue;
		if (EnemyTankAll[pTankCount]->GetCurrentCommandState() != EAIUnitCommandState::IDLE)
			continue;
		if (EnemyTankAll[pTankCount]->GetDefenseMode())
			continue;
		if(CurrentRemainSquad<=0&&CurrentRemainTank==-1)
			EnemyTankAll[pTankCount]->FindPath(OccupiedLocationStruct.OccupiedLocation);
		else
			EnemyTankAll[pTankCount]->FindPath(OccupiedLocationStruct.TankLocation[CurrentRemainTank<0?CurrentRemainTank=0:CurrentRemainTank]);
		EnemyTankAll[pTankCount]->SetDefenseMode(false);
		CurrentRemainTank++;
	}
}
void AEnemyManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UserControlUI->RemoveFromParent();
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayTimeSeconds+=DeltaTime;

}

