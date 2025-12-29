// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/ItemWidget.h"

#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UDataTable* itemdatatable = InventoryComponent->GetDataTableByItemType(InventorySlot.ItemType);
	if (!itemdatatable) return; 
	const FItemData* ItemRow = itemdatatable->FindRow<FItemData>(InventorySlot.ItemID, TEXT(""));
	if (!ItemRow) { 
		//UE_LOG(LogTemp, Warning, TEXT("ItemID not found in DataTable: %s"), *Item.ItemID.ToString()); 
		return; 
	}
	if (SizeBox_21)
	{
		SizeBox_21->SetWidthOverride(CellSize);
		SizeBox_21->SetHeightOverride(CellSize);
	}
	if (Image_342){
		Image_342->SetBrushFromTexture(ItemRow->Thumbnail, true);
		//Image_342->SetVisibility(ESlateVisibility::Visible);
	}
	if (TextBlock_0) {
		TextBlock_0->SetText(ItemRow->ItemName);
	}
}