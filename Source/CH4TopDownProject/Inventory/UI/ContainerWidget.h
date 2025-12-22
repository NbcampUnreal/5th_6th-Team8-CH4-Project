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

UCLASS()
class CH4TOPDOWNPROJECT_API UContainerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> Items;

private:
	EItemType GetItemTypeFromItemID(FName ItemID);
public:

};
