// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserControlUI.generated.h"

/**
 * 
 */
UCLASS()
class MAVERICK_API UUserControlUI : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UUserControlIcon* WB_Move;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UUserControlIcon* WB_Stop;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UUserControlIcon* WB_Attack;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UTextBlock* SoldierCount;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	class UTextBlock* TankCount;
	FLinearColor ActivateColor;
	FLinearColor DeactivateColor;
	FLinearColor ClickedColor;

	protected:
	virtual void NativeConstruct() override;
	public:
	//이동 아이콘
	//이동 아이콘 빨간색
	UFUNCTION(BlueprintCallable)
	void SetMoveActivateColor();
	//이동 아이콘 초록색
	UFUNCTION(BlueprintCallable)
	void SetMoveDeactivateColor();
	//이동 아이콘 갈색
	UFUNCTION(BlueprintCallable)
	void SetMoveClickedColor();

	//공격 아이콘
	//공격 아이콘 빨간색
	UFUNCTION(BlueprintCallable)
	void SetAttackActivateColor();
	//공격 아이콘 초록색
	UFUNCTION(BlueprintCallable)
	void SetAttackDeactivateColor();
	//공격 아이콘 갈색
	UFUNCTION(BlueprintCallable)
	void SetAttackClickedColor();

	//중지 아이콘
	//중지 아이콘 빨간색
	UFUNCTION(BlueprintCallable)
	void SetStopActivateColor();
	//중지 아이콘 초록색
	UFUNCTION(BlueprintCallable)
	void SetStopDeactivateColor();

	//화면에 표시할 SoldierCount 설정
	UFUNCTION(BlueprintCallable)
	void SetSoldierCount(int Amount);
	//화면에 표시할 TankCount 설정
	UFUNCTION(BlueprintCallable)
	void SetTankCount(int Amount);
};
