// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryComponent.h"
#include "ItemWidget.generated.h"


class USizeBox;
class UImage;
class UTextBlock;
class UBorder;
/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	/* ===== BindWidget ===== */

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox_21;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border_0;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_342;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_0;

	/* ===== Data ===== */

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI", meta = (ExposeOnSpawn = "true"))
	FInventorySlot InventorySlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI", meta = (ExposeOnSpawn = "true"))
	float CellSize = 64.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI", meta = (ExposeOnSpawn = "true"))
	UInventoryComponent* InventoryComponent;
};
