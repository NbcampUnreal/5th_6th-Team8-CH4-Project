// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/ItemData/ItemData.h"
#include "RandomItemSpawnManager.generated.h"


class AChest;

UCLASS()
class CH4TOPDOWNPROJECT_API ARandomItemSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARandomItemSpawnManager();

	UPROPERTY(EditAnywhere, Category="DataTable")
	UDataTable* WeaponAmmoTableDT;

	UPROPERTY(EditAnywhere, Category="DataTable")
	UDataTable* WeightedItemTableDT;


	TArray<FWeaponAmmoSet> CachedWeaponSets;
	
	TMap<EItemType, TArray<FWeightedItemRow>> CachedItemTable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	int32 MaxSlots = 8;
	

	void GenerateItemsForChest(AChest* Chest);

	void AddWeaponAmmoSet(TMap<FName, int32>& ResultMap);
	void AddRandomNonWeaponItem(TMap<FName, int32>& ResultMap);

	const FWeaponAmmoSet& SelectWeaponSet() const;
	const FWeightedItemRow& SelectWeightedItem(EItemType Type) const;

	EItemType GetRandomNonWeaponType() const;
	FName GetRandomAmmoID(const FWeaponAmmoSet& Set) const;
	int32 GetRandomAmmoCount() const;
};
