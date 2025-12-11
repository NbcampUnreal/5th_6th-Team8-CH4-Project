// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemData/ItemData.h"
#include "Blueprint/UserWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Items.SetNum(GetInventorytSize());

	if (InventoryWidgetClass)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());

		if (PlayerController)
		{
			InventoryWidget = CreateWidget<UUserWidget>(PlayerController, InventoryWidgetClass);
		}
	}
	
}

void UInventoryComponent::AddItem(FName ItemID)
{
	if (ItemID == NAME_None)
	{
		return;
	}

	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].ItemID == NAME_None)
		{
			Items[i].ItemID = ItemID;

			if (GEngine)
			{
				FString const Msg = FString::Printf(TEXT("Item[ %d ] slot [ %s ] store"), i, *ItemID.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Msg);
			}

			return;
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Inventory is Full"));
	}
}

void UInventoryComponent::DropItem(FName ItemID)
{
}

int32 UInventoryComponent::GetInventorytSize()
{
	if (ItemDataTable == nullptr)
	{
		return DefaultInventorySize;
	}

	if (EquipmentBagID.IsNone())
	{
		return DefaultInventorySize;
	}

	const FItemData* ItemRow = ItemDataTable->FindRow<FItemData>(EquipmentBagID, TEXT("ItemID"));

	if (ItemRow == nullptr)
	{
		return DefaultInventorySize;
	}

	return ItemRow->ContainerSize;
}


void UInventoryComponent::SetEquipmentBagID(FName NewID) { 
	int32 curInventorySize = GetInventorytSize();
	EquipmentBagID = NewID; 
	int32 nextInventorySize = GetInventorytSize();

	Items.SetNum(nextInventorySize);
}

