// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/OccupiedComponent.h"

UOccupiedComponent::UOccupiedComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOccupiedComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OccupyingTime = 0.f;
	IsOccupiedNumber = 0;
	bIsOccupied = false;
	
	OnComponentBeginOverlap.AddDynamic(this, &UOccupiedComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UOccupiedComponent::OnEndOverlap);
}

void UOccupiedComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsOccupied == true)	return;
	
	if((bIsOccupied == false) && (IsOccupiedNumber < 1))
	{
		if(OccupyingTime <= 0.f)	return;
		OccupyingTime -= DeltaTime;
		GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OccupyingTime: %f"), OccupyingTime));
	}
	else
	{
		OccupyingTime += DeltaTime;
		GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OccupyingTime: %f"), OccupyingTime));
		if(OccupyingTime >= OccupiedTime)
		{
			bIsOccupied = true;
			GEngine -> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Occupied!")));
		}
	}
}

void UOccupiedComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag("Player"))
	{
		IsOccupiedNumber++;
	}
}

void UOccupiedComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->ActorHasTag("Player"))
	{
		IsOccupiedNumber--;
	}
}
