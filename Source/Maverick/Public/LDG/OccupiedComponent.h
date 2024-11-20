// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "OccupiedComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class MAVERICK_API UOccupiedComponent : public UBoxComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Occupied", meta = (AllowPrivateAccess = "true"))
	float OccupiedTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Occupied", meta = (AllowPrivateAccess = "true"))
	bool bIsOccupied;
	
	float OccupyingTime;
	int IsOccupiedNumber;



public:
	UOccupiedComponent();

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	bool GetIsOccupied() const { return bIsOccupied; }

	UFUNCTION(BlueprintCallable)
	float GetOccupiedPercent() const { return FMath::Clamp((OccupyingTime / OccupiedTime), 0.f, 100.f); }
};
