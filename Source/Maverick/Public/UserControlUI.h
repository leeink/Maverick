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
	//�̵� ������
	//�̵� ������ ������
	UFUNCTION(BlueprintCallable)
	void SetMoveActivateColor();
	//�̵� ������ �ʷϻ�
	UFUNCTION(BlueprintCallable)
	void SetMoveDeactivateColor();
	//�̵� ������ ����
	UFUNCTION(BlueprintCallable)
	void SetMoveClickedColor();

	//���� ������
	//���� ������ ������
	UFUNCTION(BlueprintCallable)
	void SetAttackActivateColor();
	//���� ������ �ʷϻ�
	UFUNCTION(BlueprintCallable)
	void SetAttackDeactivateColor();
	//���� ������ ����
	UFUNCTION(BlueprintCallable)
	void SetAttackClickedColor();

	//���� ������
	//���� ������ ������
	UFUNCTION(BlueprintCallable)
	void SetStopActivateColor();
	//���� ������ �ʷϻ�
	UFUNCTION(BlueprintCallable)
	void SetStopDeactivateColor();

	//ȭ�鿡 ǥ���� SoldierCount ����
	UFUNCTION(BlueprintCallable)
	void SetSoldierCount(int Amount);
	//ȭ�鿡 ǥ���� TankCount ����
	UFUNCTION(BlueprintCallable)
	void SetTankCount(int Amount);
};
