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
#include "XR_LSJ/ResultIconWidget.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
void AEnemyManager::DieSoldier()
{
	if(EndGame || SoldierCount<=0)
		return;
	SoldierCount--;
	EnemyCountWidget->SetSoldierCount(SoldierCount);


	if (SoldierCount <= 0 && TankCount <= 0)
	{
		/*EndGame = true;
		
		if (GameResultIconClass)
		{
			ResultIconWidget = Cast<UResultIconWidget>(CreateWidget(GetWorld(), GameResultIconClass));
			ResultIconWidget->SetVisibleIcon(true);
			ResultIconWidget->AddToViewport();
		}
		FTimerHandle ShowResultHandle;
				GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,[&]()
			{
				ShowResult(true);
			}, 3.0f, false);*/
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
		if (GameResultIconClass)
		{
			ResultIconWidget = Cast<UResultIconWidget>(CreateWidget(GetWorld(),GameResultIconClass));
			ResultIconWidget->SetVisibleIcon(false);
			ResultIconWidget->AddToViewport();
		}
		FTimerHandle ShowResultHandle;
				GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,[&]()
			{
				ShowResult(false);
			}, 3.0f, false);
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
		
		if (GameResultIconClass)
		{
			ResultIconWidget = Cast<UResultIconWidget>(CreateWidget(GetWorld(),GameResultIconClass));
			ResultIconWidget->SetVisibleIcon(false);
			ResultIconWidget->AddToViewport();
		}
		FTimerHandle ShowResultHandle;
		GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,[&]()
			{
				ShowResult(false);
			}, 3.0f, false);
	}
}
FString AEnemyManager::ConvertSecondsToMinutesAndSeconds(int32 TotalSeconds)
{
    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;
    
    // 두 자리 형식을 유지하려면 추가 포맷
    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}
void AEnemyManager::ShowResult(bool Value)
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
		GameResultWidget->SetResultIcon(Value);
		FString Time = ConvertSecondsToMinutesAndSeconds(PlayTimeSeconds);
		GameResultWidget->SetClearTime(Time);
		GameResultWidget->AddToViewport();
	}
}

void AEnemyManager::DieTank()
{
	if(EndGame || TankCount<=0)
		return;
	TankCount--;
	EnemyCountWidget->SetTankCount(TankCount);

	if (SoldierCount <= 0 && TankCount <= 0)
	{
		/*EndGame = true;
		if (GameResultIconClass)
		{
			ResultIconWidget = Cast<UResultIconWidget>(CreateWidget(GetWorld(), GameResultIconClass));
			ResultIconWidget->SetVisibleIcon(true);
			ResultIconWidget->AddToViewport();
		}
		FTimerHandle ShowResultHandle;
				GetWorld()->GetTimerManager().SetTimer(ShowResultHandle,[&]()
			{
				ShowResult(true);
			}, 3.0f, false);*/
	}
}
// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	OccupiedTop = true;
	OccupiedBottom = true;
	EndGame = false;
	if (EnemyCountClass)
	{
		EnemyCountWidget = Cast<UEnemyCount>(CreateWidget(GetWorld(),EnemyCountClass));
		EnemyCountWidget->AddToViewport();
	}

	TArray<AActor*> FoundSquadActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASquadManager::StaticClass(), FoundSquadActors);
	for (AActor* SquadActor : FoundSquadActors)
	{
		if (ASquadManager* SquadManager = Cast<ASquadManager>(SquadActor))
		{
			SquadManager->SetDefenseMode(false);
			SquadManager->FDelSoldierUnitDie.BindUFunction(this, FName("DieSoldier"));
			SquadManager->SetMinimapUIZOrder(SoldierCount);
			SoldierCount += SquadManager->GetCurrentSquadCount();
		}
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
void AEnemyManager::AddTank(ATankBase* Tank)
{
	Tank->Del_PlayerTankUnitDie.BindUFunction(this, FName("DiePlayerTank"));
	MaxPlayerTankCount++;
	PlayerTankCount++;
	UserControlUI->SetTankCount(PlayerTankCount);
}
void AEnemyManager::AddSoldier(ASoldier* Soldier)
{
	Soldier->Del_PlayerSoldierUnitDie.BindUFunction(this, FName("DiePlayerSoldier"));
	MaxPlayerSoldierCount++;
	PlayerSoldierCount++;
	UserControlUI->SetSoldierCount(PlayerSoldierCount);
}
void AEnemyManager::UpdateOccupiedLocationStruct(FOccupiedLocationStruct& pOccupiedLocationStruct,AOccupiedLocation* pOccupiedLocationActor)
{
	switch (pOccupiedLocationActor->GetOccupiedLocationType())
	{
	case EOccupiedLocationType::Occupation:
		pOccupiedLocationStruct.OccupiedLocation = pOccupiedLocationActor->GetActorLocation();
		break;
	case EOccupiedLocationType::Tank:
		//UE_LOG(LogTemp,Error,TEXT(" %s CurrentRemainTank %s"),*pOccupiedLocationActor->GetName(),*pOccupiedLocationActor->GetActorLocation().ToString());
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
	//왜 이도ㅇ을 안하지?
		//중간에 CheckPlayerUnitLocation이 실행되서
		//LandScape에 충돌판정이 되어서
	//아래가 점령되었다면
	if (false == OccupiedBottom)
	{
		//최소 방어 분대 배치 체크
		//MinDefensiveDeployment(BottomOccupiedLocationStruct);
		UE_LOG(LogTemp,Error,TEXT("TopOccupiedLocationStruct"));
		//위쪽을 점령한다.
		MoveToTakeOver(TopOccupiedLocationStruct);
	}
	//아래가 점령 되지 않았다면
	else if (true == OccupiedBottom)
	{
		//최소 방어 분대 배치 체크
		//MinDefensiveDeployment(TopOccupiedLocationStruct);
		UE_LOG(LogTemp,Error,TEXT("BottomOccupiedLocationStruct"));
		//아래쪽을 점령한다.
		MoveToTakeOver(BottomOccupiedLocationStruct);
	}
}
//최소 방어 배치
void AEnemyManager::MinDefensiveDeployment(FOccupiedLocationStruct& OccupiedLocationStruct)
{
		int32 UnitCount = 0;
		//가까운 분대를 배치 시킨다. 분대가 없다면 탱크를 배치한다.
		if(OccupiedLocationStruct.SquadLocation.Num()<=0 ||  OccupiedLocationStruct.DefensiveUnit.Num()<=0)
			return;

		for (int32 DefensiveUnitCount = 0; DefensiveUnitCount < OccupiedLocationStruct.DefensiveUnit.Num(); DefensiveUnitCount++)
		{
			if (OccupiedLocationStruct.DefensiveUnit[DefensiveUnitCount] == false)
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
				CloseFirstSquad->CheckLocationForObject(OccupiedLocationStruct.SquadLocation[DefensiveUnitCount]);
				CloseFirstSquad->SetCommandState(EAIUnitCommandState::Defense);
				OccupiedLocationStruct.DefensiveUnit[DefensiveUnitCount] = true;
				UnitCount++;
			}
		}
		//if (UnitCount <= 0 && OccupiedLocationStruct.DefensiveUnit.Num()>0) //탱크를 배치
		//{
		//	if (OccupiedLocationStruct.DefensiveUnit[0] == false)
		//	{
		//		float MinDistance = 100000.f;
		//		AAITankPawn* CloseFirstSquad = nullptr;
		//		for (int32 pTankCount = 0; pTankCount < EnemyTankAll.Num(); pTankCount++)
		//		{
		//			if (EnemyTankAll[pTankCount]->GetCurrentCommandState() == EAIUnitCommandState::DIE)
		//				continue;
		//			if (EnemyTankAll[pTankCount]->GetDefenseMode())
		//				continue;
		//			float DefenseDistance = FVector::Distance(OccupiedLocationStruct.OccupiedLocation, EnemyTankAll[pTankCount]->GetActorLocation());
		//			
		//			if (MinDistance > DefenseDistance)
		//			{
		//				MinDistance = DefenseDistance;
		//				CloseFirstSquad = EnemyTankAll[pTankCount];
		//			}
		//		}
		//		if (nullptr == CloseFirstSquad)
		//			return;
		//		CloseFirstSquad->SetDefenseMode(true);
		//		FVector Location = OccupiedLocationStruct.OccupiedLocation;
		//		Location.Z = 350.0f;
		//		CloseFirstSquad->FindPath(Location);
		//		OccupiedLocationStruct.DefensiveUnit[0] = true;
		//	}
		//}
}
//점령 이동
void AEnemyManager::MoveToTakeOver(FOccupiedLocationStruct& OccupiedLocationStruct)
{
	int32 CurrentRemainSquad = 0;
	int32 CurrentRemainTank = 0;

	if(OccupiedLocationStruct.SquadLocation.Num()<=0)
		return;
	
	for (int32 pSquadManagerCount = 0;pSquadManagerCount<EnemySquadAll.Num(); pSquadManagerCount++)
	{
		if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState()==EAIUnitCommandState::DIE)
			continue;
		/*if (EnemySquadAll[pSquadManagerCount]->GetCurrentCommandState()==EAIUnitCommandState::Defense)
			continue;*/

		EnemySquadAll[pSquadManagerCount]->CheckLocationForObject(OccupiedLocationStruct.SquadLocation[CurrentRemainSquad++]);
		if(OccupiedLocationStruct.SquadLocation.Num()<= CurrentRemainSquad)
			break;
	}
	if(OccupiedLocationStruct.TankLocation.Num()<=0)
		return;

	
	for (int32 pTankCount = 0;pTankCount<EnemyTankAll.Num(); pTankCount++)
	{
		if (EnemyTankAll[pTankCount]->GetCurrentCommandState()==EAIUnitCommandState::DIE)
			continue;
		if (EnemyTankAll[pTankCount]->GetCurrentCommandState() != EAIUnitCommandState::IDLE)
			continue;
		//if (EnemyTankAll[pTankCount]->GetDefenseMode())
		//	continue;
		EnemyTankAll[pTankCount]->FindPath(OccupiedLocationStruct.TankLocation[CurrentRemainTank++]);
		//UE_LOG(LogTemp,Error,TEXT("pTankCount %d CurrentRemainTank %s"),pTankCount,*OccupiedLocationStruct.TankLocation[CurrentRemainTank<0?CurrentRemainTank=0:CurrentRemainTank].ToString());
		if(OccupiedLocationStruct.TankLocation.Num()<= CurrentRemainTank)
			break;
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

