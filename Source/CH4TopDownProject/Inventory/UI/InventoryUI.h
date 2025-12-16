// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

/**
 * 
 */
class UInventoryComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* OwnerInventoryComponent;
};
