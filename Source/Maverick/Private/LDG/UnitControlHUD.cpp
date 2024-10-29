// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/UnitControlHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LDG/OperatorPawn.h"
#include "LDG/Soldier.h"
#include "LDG/TankBase.h"

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
		GetActorsInSelectionRectangle(ATankBase::StaticClass(), StartMousePosition, CurrentMousePosition, InRectangleTanks, false, false);

		// Unit Selection -----------------------------------------------
		for(auto* Unit: InRectangleUnits)
		{
			if(auto* SoldierUnit = Cast<ASoldier>(Unit))
			{
				SoldierUnit -> Selected();
				OperatorPawn -> GetSelectedUnits().AddUnique(SoldierUnit);
			}
		}
		
		for(auto* Unit: InRectangleTanks)
		{
			if(auto* TankUnit = Cast<ATankBase>(Unit))
			{
				TankUnit -> Selected();
				OperatorPawn -> GetSelectedTanks().AddUnique(TankUnit);
			}
		}
		// Unit Selection -----------------------------------------------

		// Unit DeSelection -----------------------------------------------
		for(auto* Unit: OperatorPawn -> GetSelectedUnits())
		{
			if(InRectangleUnits.Find(Unit) == -1)
			{
				Unit -> Deselected();
				Cast<ASoldier>(Unit) -> ToggleWidget(false);
				OperatorPawn -> GetSelectedUnits().Remove(Unit);
			}
			
		}

		for(auto* Unit: OperatorPawn -> GetSelectedTanks())
		{
			if(InRectangleTanks.Find(Unit) == -1)
			{
				Unit -> Deselected();
				OperatorPawn -> GetSelectedTanks().Remove(Unit);
			}
		}
		// Unit DeSelection -----------------------------------------------
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

	if(InRectangleUnits.Num() > 0)
	{
		Cast<ASoldier>(InRectangleUnits[FMath::RandHelper(InRectangleUnits.Num() - 1)]) -> ToggleWidget(true);
	}
}
