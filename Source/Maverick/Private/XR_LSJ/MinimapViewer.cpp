// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/MinimapViewer.h"
#include "XR_LSJ/MinimapWidget.h"

// Sets default values
AMinimapViewer::AMinimapViewer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMinimapViewer::BeginPlay()
{
	Super::BeginPlay();
	if (MinimapUIClass)
	{
		MinimapUI = Cast<UMinimapWidget>(CreateWidget(GetWorld(),MinimapUIClass));
		MinimapUI->AddToViewport();
	}
}

void AMinimapViewer::CreateWarningUI(FVector Location)
{
	if(MinimapUI)
		MinimapUI->CreateWarningUI(Location);
}

// Called every frame
void AMinimapViewer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

