// Fill out your copyright notice in the Description page of Project Settings.


#include "LDG/RifleSoliderAnimInstance.h"

void URifleSoliderAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void URifleSoliderAnimInstance::PlayAttackMontage()
{
	if(AttackMontage != nullptr)
	{
		Montage_Play(AttackMontage);
	}
}

void URifleSoliderAnimInstance::PlayDeathMontage()
{
	if(DeathMontage != nullptr)
	{
		Montage_Play(DeathMontage);	
	}
}
