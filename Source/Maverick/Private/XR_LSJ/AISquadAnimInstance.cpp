              // Fill out your copyright notice in the Description page of Project Settings.


#include "XR_LSJ/AISquadAnimInstance.h"
#include "XR_LSJ/AISquad.h"
#include "Animation/AnimMontage.h"

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
void UAISquadAnimInstance::PlayFireMontage()
{
	IsAttacking = true;
	Montage_Play(AttackAM);
}
void UAISquadAnimInstance::PlayDieMontage()
{
	IsAttacking = false;
	Montage_Play(DieAM,2.0f);
}
void UAISquadAnimInstance::StopFireMontage()
{
	Montage_Stop(0.5f,AttackAM);
}
void UAISquadAnimInstance::AnimNotify_Fire()
{
	if (AISquadBody)
	{
		AISquadBody->FSMComp->SetAttackCurrentTime(0.f);
		AISquadBody->AttackFire();
	}	
}

