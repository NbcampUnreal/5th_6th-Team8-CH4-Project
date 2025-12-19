// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ItemData/ItemData.h"
#include "BaseItemComponent.generated.h"

struct FInventorySlot;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UBaseItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseItemComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	int32 Num;

	UFUNCTION(BlueprintCallable)
	FInventorySlot GetItemData();
};
