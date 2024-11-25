// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UENUM(BlueprintType)
enum class EPlayerUnitPathDirection : uint8
{
	Left,
	Middle,
	Right
};

USTRUCT(Atomic,BlueprintType)
struct FOccupiedLocationStruct
{
	GENERATED_USTRUCT_BODY()
public:
	//점령 위치
	//탱크와 분대 둘 다 위치할 수 있다.
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector OccupiedLocation;
	//탱크 위치
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FVector> TankLocation;
	//분대 위치
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FVector> SquadLocation;
	//최소 방어 유닛 배치 여부
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<bool> DefensiveUnit;
};

UCLASS()
class MAVERICK_API AEnemyManager : public AActor
{
	GENERATED_BODY()

	//적의 수 표시하는 위젯
	UPROPERTY()
	class UEnemyCount* EnemyCountWidget;
	//게임 결과창 위젯
	UPROPERTY()
	class UGameResultWidget* GameResultWidget;
	UPROPERTY()
	class UResultIconWidget* ResultIconWidget;
	//플레이어 유닛 수와 명령을 표시하는 위젯
	UPROPERTY()
	class UUserControlUI* UserControlUI;
	//적 탱크 모든 부대
	UPROPERTY()
	TArray<class AAITankPawn*> EnemyTankAll;
	UPROPERTY()
	TArray<class ASquadManager*> EnemySquadAll;
	UPROPERTY()
	TArray<class ATankBase*> PlayerTankAll;
	UPROPERTY()
	TArray<class ASoldier*> PlayerSquadAll;

	//최대 적 보병의 수
	int32 MaxSoldierCount;
	//최대 적 탱크의 수
	int32 MaxTankCount;
	//현재 적 보병의 수
	int32 SoldierCount;
	//현재 적 탱크의 수
	int32 TankCount;

	//최대 플레이어 보병의 수
	int32 MaxPlayerSoldierCount;
	//최대 플레이어 탱크의 수
	int32 MaxPlayerTankCount;
	//현재 플레이어 보병의 수
	int32 PlayerSoldierCount;
	//현재 플레이어 탱크의 수
	int32 PlayerTankCount;
	//플레이 타임 초
	float PlayTimeSeconds;
	bool EndGame;
	
	//위쪽의 점령지가 플레이어에게 점령 되었는지 여부
	bool OccupiedTop;
	//아래쪽의 점령지가 플레이어에게 점령 되었는지 여부
	bool OccupiedBottom;
	//적들이 위치할 위쪽의 점령지에서의 위치정보
	FOccupiedLocationStruct TopOccupiedLocationStruct;
	//적들이 위치할 아래쪽의 점령지에서의 위치정보
	FOccupiedLocationStruct BottomOccupiedLocationStruct;
	//플레이어 유닛의 위치를 확인하는 FTimerHandle
	FTimerHandle CheckPlayerUnitLocationHandle;
public:	
	// Sets default values for this actor's properties
	AEnemyManager();
	//적 보병이 죽었을 때 화면에 표시된 현재 적 보병의 수를 동기화
	UFUNCTION()
	void DieSoldier();
	//플레이어 보병이 죽었을 때 화면에 표시된 현재 플레이어 보병의 수를 동기화
	UFUNCTION()
	void DiePlayerSoldier();
	//플레이어 탱크가 죽었을 때 화면에 표시된 현재 플레이어 탱크의 수를 동기화
	UFUNCTION()
	void DiePlayerTank();
	//적 탱크가 죽었을 때 화면에 표시된 현재 적 탱크의 수를 동기화
	UFUNCTION()
	void DieTank();
	//플레이 타임(초)를 MM:SS로 변환
	FString ConvertSecondsToMinutesAndSeconds(int32 TotalSeconds);
	//플레이어와 적의 닉네임과 처치한 적, 처치된 아군 수를 표시하는 결과창을 띄운다.
	UFUNCTION(BlueprintCallable)
	void ShowResult(bool Value);

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> PlayerControllerClass;
	//화면에 Enemy count 표시
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> EnemyCountClass;
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> GameResultClass;
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<UUserWidget> GameResultIconClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ASquadManager> SquadManagerClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAITankPawn> TankPawnClass;
		UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ATankBase> PlayerTankPawnClass;
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class ASoldier> PlayerSoldierPawnClass;

	//위쪽 점령지
	UFUNCTION(BlueprintCallable)
	void SetOccupiedTop(bool Value){OccupiedTop=Value;}
	//아래쪽 점령지
	UFUNCTION(BlueprintCallable)
	void SetOccupiedBottom(bool Value){OccupiedBottom=Value;}
protected:

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void UpdateOccupiedLocationStruct(FOccupiedLocationStruct& pOccupiedLocationStruct, class AOccupiedLocation* pOccupiedLocationActor);
	void CheckPlayerUnitLocation();
	void MinDefensiveDeployment(FOccupiedLocationStruct& OccupiedLocationStruct);
	void MoveToTakeOver(FOccupiedLocationStruct& OccupiedLocationStruct);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//탱크 추가시 실행해야하는 함수
	void AddTank(class ATankBase* Tank);
};
