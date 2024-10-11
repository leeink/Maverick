// Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadAnimInstance.h"
#include "XR_LSJ/AISquad.h"

void UAISquadAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (nullptr != AISquadBody)
	{
		CurrentState = AISquadBody->FSMComp->GetCurrentState();
	}
}

void UAISquadAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	AISquadBody = Cast<AAISquad>(TryGetPawnOwner());

}
