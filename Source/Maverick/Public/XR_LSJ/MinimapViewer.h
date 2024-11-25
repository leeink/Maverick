// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapViewer.generated.h"

UCLASS()
class MAVERICK_API AMinimapViewer : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,Category = "Widget")
	TSubclassOf<class UUserWidget> MinimapUIClass;
	class UMinimapWidget* MinimapUI;
public:	
	// Sets default values for this actor's properties
	AMinimapViewer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CreateWarningUI(FVector Location);
};
