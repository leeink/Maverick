// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetState.generated.h"

enum class EState : uint8;
/**
 * 
 */
UCLASS()
class MAVERICK_API UBTTask_SetState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetState();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EState NewState;
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
