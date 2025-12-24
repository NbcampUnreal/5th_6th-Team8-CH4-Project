// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/WorldItemBase.h"
#include "Chest.generated.h"

/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API AChest : public AWorldItemBase
{
	GENERATED_BODY()

	

public:
	AChest();

	UPROPERTY(EditAnywhere)
	TMap<FName,int32> ItemList;

	virtual void Interact_Implementation(AActor* Interactor) override;

	void SetItem(const TMap<FName, int32>& ItemMap);
	
};
