// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/UnitControlHUD.h"

#include "Kismet/GameplayStatics.h"
#include "LDG/OperatorPawn.h"
#include "LDG/Soldier.h"

AUnitControlHUD::AUnitControlHUD()
{
	
}

void AUnitControlHUD::BeginPlay()
{
	Super::BeginPlay();

	OperatorPawn = Cast<AOperatorPawn>(UGameplayStatics::GetPlayerController(GetWorld(), 0) -> GetPawn());
}

void AUnitControlHUD::DrawHUD()
{
	Super::DrawHUD();

	if(bIsDrawing)
	{
		FLinearColor Color = FLinearColor(.14f,1.f, .02f, .15f);
		DrawRect(Color, StartMousePosition.X, StartMousePosition.Y, CurrentMousePosition.X - StartMousePosition.X, CurrentMousePosition.Y - StartMousePosition.Y);

		GetActorsInSelectionRectangle(ASoldier::StaticClass(), StartMousePosition, CurrentMousePosition, InRectangleUnits, false, false);

		for(auto* Unit: InRectangleUnits)
		{
			auto* InnerUnit = Cast<ASoldier>(Unit);
			InnerUnit -> Selected();

			OperatorPawn -> GetSelectedUnits().AddUnique(InnerUnit);
		}

		for(auto* Unit: OperatorPawn -> GetSelectedUnits())
		{
			if(InRectangleUnits.Find(Unit) == -1)
			{
				Unit -> Deselected();
				OperatorPawn -> GetSelectedUnits().Remove(Unit);
			}
		}
	}
}

void AUnitControlHUD::MarqueePressed()
{
	bIsDrawing = true;
	
	float mouseX;
	float mouseY;
	
	GetWorld() -> GetFirstPlayerController() -> GetMousePosition(mouseX, mouseY);
	StartMousePosition = FVector2D(mouseX, mouseY);
	CurrentMousePosition = StartMousePosition;
}

void AUnitControlHUD::MarqueeHeld()
{
	float mouseX;
	float mouseY;
	
	GetWorld() -> GetFirstPlayerController() -> GetMousePosition(mouseX, mouseY);
	CurrentMousePosition = FVector2D(mouseX, mouseY);
}

void AUnitControlHUD::MarqueeReleased()
{
	bIsDrawing = false;
}
