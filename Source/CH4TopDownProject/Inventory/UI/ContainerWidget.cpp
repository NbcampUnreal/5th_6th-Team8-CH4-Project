// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/ContainerWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Interactor/Chest.h"


EItemType UContainerWidget::GetItemTypeFromItemID(FName ItemID)
{
	if (ItemID.IsNone())
	{
		return EItemType::None;
	}

	const FString IDString = ItemID.ToString();

	if (IDString.Len() == 0)
	{
		return EItemType::None;
	}

	const TCHAR FirstChar = IDString[0];

	if (!FChar::IsDigit(FirstChar))
	{
		return EItemType::None;
	}

	const int32 TypeValue = FirstChar - '0';

	// enum 범위 체크
	if (TypeValue < static_cast<int32>(EItemType::None) ||
		TypeValue > static_cast<int32>(EItemType::Weapon))
	{
		return EItemType::None;
	}

	return static_cast<EItemType>(TypeValue);
}

void UContainerWidget::ChestItemEntryToInventorySlot(TArray<FChestItemEntry> ChestItems)
{
	Items.Empty();

	for (const FChestItemEntry& ChestItem : ChestItems)
	{
		/*if (ChestItem.ItemName == NAME_None || ChestItem.ItemNum <= 0)
		{
			continue;
		}*/

		FInventorySlot NewSlot;
		NewSlot.ItemID = ChestItem.ItemName;
		NewSlot.Num = ChestItem.ItemNum;
		NewSlot.ItemType = GetItemTypeFromItemID(ChestItem.ItemName);

		Items.Add(NewSlot);
	}
}
