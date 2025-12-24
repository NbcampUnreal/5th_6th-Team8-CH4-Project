// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/RandomItemSpawnManager.h"

#include "Interactor/Chest.h"
#include "Kismet/GameplayStatics.h"


ARandomItemSpawnManager::ARandomItemSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ARandomItemSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	TArray<FWeaponAmmoSet*> AmmoSetRows;
	WeaponAmmoTableDT->GetAllRows(TEXT("WeaponAmmo"), AmmoSetRows);

	for (auto* Row : AmmoSetRows)
	{
		CachedWeaponSets.Add(*Row);
	}


	TArray<FWeightedItemRow*> ItemRows;
	WeightedItemTableDT->GetAllRows(TEXT("WeightedItems"), ItemRows);

	for (auto* Row : ItemRows)
	{
		CachedItemTable.FindOrAdd(Row->ItemType).Add(*Row);
	}

	TArray<AActor*> Chests;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChest::StaticClass(), Chests);

	for (AActor* Actor : Chests)
	{
		if (AChest* Chest = Cast<AChest>(Actor))
		{
			GenerateItemsForChest(Chest);
		}
	}
}

void ARandomItemSpawnManager::GenerateItemsForChest(AChest* Chest)
{
	TMap<FName, int32> ResultMap;

	int32 TargetSlots = FMath::RandRange(2, MaxSlots);
	int32 UsedSlots = 0;


	AddWeaponAmmoSet(ResultMap);
	UsedSlots += 2;


	int32 RemainingSlots = TargetSlots - UsedSlots;

	while (RemainingSlots--)
	{
		AddRandomNonWeaponItem(ResultMap);
	}

	Chest->SetItem(ResultMap);
}

void ARandomItemSpawnManager::AddWeaponAmmoSet(TMap<FName, int32>& ResultMap)
{
	const FWeaponAmmoSet& Set = SelectWeaponSet();


	ResultMap.Add(Set.WeaponID, 1);

	FName AmmoID = GetRandomAmmoID(Set);
	int32 AmmoCount = GetRandomAmmoCount();

	ResultMap.Add((AmmoID), AmmoCount);
}

void ARandomItemSpawnManager::AddRandomNonWeaponItem(TMap<FName, int32>& ResultMap)
{
	EItemType Type = GetRandomNonWeaponType();
	const FWeightedItemRow& Item = SelectWeightedItem(Type);

	int32 Count = FMath::RandRange(Item.MinCount, Item.MaxCount);
	ResultMap.FindOrAdd(Item.ItemID) += Count;
}

const FWeaponAmmoSet& ARandomItemSpawnManager::SelectWeaponSet() const
{
	float TotalWeight = 0.f;
	for (const auto& Set : CachedWeaponSets)
	{
		TotalWeight += Set.Weight;
	}

	float Rand = FMath::FRandRange(0.f, TotalWeight);

	float Acc = 0.f;
	for (const auto& Set : CachedWeaponSets)
	{
		Acc += Set.Weight;
		if (Rand <= Acc)
		{
			return Set;
		}
	}

	return CachedWeaponSets.Last();
}

const FWeightedItemRow& ARandomItemSpawnManager::SelectWeightedItem(EItemType Type) const
{
	const TArray<FWeightedItemRow>& Table = CachedItemTable[Type];

	float TotalWeight = 0.f;
	for (const auto& Item : Table)
	{
		TotalWeight += Item.Weight;
	}

	float Rand = FMath::FRandRange(0.f, TotalWeight);

	float Acc = 0.f;
	for (const auto& Item : Table)
	{
		Acc += Item.Weight;
		if (Rand <= Acc)
		{
			return Item;
		}
	}

	return Table.Last();
}


EItemType ARandomItemSpawnManager::GetRandomNonWeaponType() const
{
	TArray<EItemType> Types = {
		EItemType::Bag,
		EItemType::Consumable,
		//EItemType::Equipment_Head,
		EItemType::Equipment_Body,
		EItemType::Ammo,
	};

	return Types[FMath::RandRange(0, Types.Num() - 1)];
}

FName ARandomItemSpawnManager::GetRandomAmmoID(const FWeaponAmmoSet& Set) const
{
	return Set.AmmoID;
}

int32 ARandomItemSpawnManager::GetRandomAmmoCount() const
{
	return FMath::RandRange(10, 30);
}
