// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XR_LSJ/AISquad.h"
#include "IAICommand.h"
#include "SquadManager.generated.h"

DECLARE_DELEGATE(FDel_SoldierUnitDie);

UENUM(BlueprintType)
enum class EObstructionDirection : uint8
{
	Left,
	Right,
	Down,
	Up
};

UCLASS()
class MAVERICK_API ASquadManager : public AActor, public IIAICommand
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class UBoxComponent* BoxComp;

	//공격 타겟
	TArray<AActor*> Target;
	//분대 능력치
	UPROPERTY()
	struct FSquadData SquadManagerAbility;
	//분대 현재 HP
	int32 CurrentSquadHp;
	//분대 최대 HP
	int32 MaxSquadHp; 
	//분대의 분대원들
	TArray<class AAISquad*> SquadArray;
	//분대원 포지션 목록
	TArray<FVector> SquadPositionArray;
	//최대 분대원 수
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 MaxSpawnCount = 6;
	//현재 살아있는 분대원 수
	int32 CurrentSquadCount = 6;
	//이동 테스트
	FVector ArrivalPoint = FVector(1500,0,0);
	//엄폐 위치 목록
	TArray<FVector> ObstructionPoints;
	//현재 부착하고 있는 SquadNumber
	int32 CurrentAttachedSquadNumber;
	//체력바 UI Component
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess = true))
	class UWidgetComponent* HpWidgetComp;

	//시작시 이동할 목표 지점
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	FVector StartGoalLocation;

	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess = true))
	class UWidgetComponent* MinimapHpWidgetComp;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* MinimapHpWidgetSpringArm;
public:
	//분대원 죽음 알림 델리게이트
	FDel_SoldierUnitDie FDelSoldierUnitDie;
	//체력바 UI Class
	UPROPERTY(EditDefaultsOnly,Category = "HpBar")
	TSubclassOf<class UHpBarNewIcon> MinimapHpWidgetClass;

	virtual FVector GetTargetLocation();
	//체력바 UI Class
	UPROPERTY(EditDefaultsOnly,Category = "HpBar")
	TSubclassOf<class UAIUnitHpBar> HpBarClass;
	//목표지점 최대 거리
	UPROPERTY(EditDefaultsOnly)
	float MaxMoveLength = 10000.0f;
	// Sets default values for this actor's properties
	ASquadManager();
	//스폰할 분대원 BP
	UPROPERTY(EditDefaultsOnly,Category = "Spawn")
	TSubclassOf<class AAISquad> SpawnSquadPactory;

	virtual EAIUnitCommandState GetCurrentCommandState() override;
	virtual void SetCommandState(EAIUnitCommandState Command) override;
	void SetMinimapUIZOrder(int32 Value);
	TArray<class AAISquad*> GetSquadArray() const { return SquadArray; }
	void SetSquadArray(TArray<class AAISquad*> val) { SquadArray = val; }
	int32 GetCurrentSquadCount() const { return CurrentSquadCount; }
	void SetCurrentSquadCount(int32 val) { CurrentSquadCount = val; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// PointA와 어느지점이 가까운지 비교하는 함수
	bool IsCloserThan(const FVector& PointA, const FVector& PointB, const FVector& PointC);
	UFUNCTION()
	void FindCloseTargetPlayerUnit();
	// SquadManagerAbility.FindTargetRange 만큼 적을 탐색하고 가까운 적 유닛을 공격
	UFUNCTION()
	void FindCloseTargetUnit();
	// SquadManagerAbility.FindTargetRange 만큼 적을 탐색하고 적 분대를 공격
	void FindTargetSquad();
	// 각 분대원에게 유닛 타겟 공격 지시
	void AttackTargetUnit();
	// 각 분대원에게 분대 타겟 공격 지시
	void AttackTargetSquad(AActor* TargetActor);
	//목표지점에서 충돌된 Actor 의 경계값 위치 가져오기 
	TArray<FVector> GetSurfacePointsOnRotatedBoundingBox(AActor* TargetActor, float Interval /*= 50.0f*/);
	TArray<FVector> GetSurfacePointsOnRotatedBoxComp(AActor* TargetActor, float Interval /*= 50.0f*/);
	//TargetLocation로 가는 경로를 찾아 분대원들에게 전달, 도착시 진형을 유지한다.
	void FindPath(const FVector& TargetLocation);
	//엄폐물로 가는 경로를 찾아 분대원들에게 전달, 도착시 진형을 유지한다.
	void FindObstructionPath(TArray<FVector>& TargetLocation);
	//TargetLocation로 이동한다. 엄폐물이 있다면 엄폐할 수 있는 가장 가까운 위치로 이동한다. 
	void CheckLocationForObject(const FVector& TargetLocation);
	//CheckLocationForObject 테스트
	void CheckLocationForObject();
	//분대원이 데미지를 받으면 체력바에 동기화
	UFUNCTION()
	void DamagedSquadUnit(float Damage);
	//체력바 표시하는 분대원이 죽었다면 살아있는 다른 분대원에게 부착
	UFUNCTION()
	void DieSquadUnit(int32 SquadNumber);
	//분대원이 이동 불가능한 위치를 받았다면 분대장 주위의 유효한 위치를 준다.
	UFUNCTION()
	void MoveToValidDestination(int32 SquadNumber);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//엄폐할 위치가 분대원 수보다 부족한 경우 위치를 생성
	void MakeObstructionPoint(AActor* TargetActor, TArray<FVector>& OutPoints, EObstructionDirection DirectionNum);

	//엄폐할 위치 생성
	void GeneratePointsBetweenTwoCorners(const FVector& P1, const FVector& P2, float Interval, TArray<FVector>& OutPoints);
	
};
