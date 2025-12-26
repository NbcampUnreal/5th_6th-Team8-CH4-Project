// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData/ItemData.h"
#include "ContainerWidget.generated.h"

/**
 * 
 */
struct FInventorySlot;
struct FChestItemEntry;
class AChest;

UCLASS()
class CH4TOPDOWNPROJECT_API UContainerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	AChest* OwnerChest;
	UPROPERTY(BlueprintReadOnly)
	TArray<FInventorySlot> Items;

private:
	UFUNCTION(BlueprintCallable)
	EItemType GetItemTypeFromItemID(FName ItemID);	
public:
	UFUNCTION(BlueprintCallable)
	void ChestItemEntryToInventorySlot(TArray<FChestItemEntry> ChestItems);
};
