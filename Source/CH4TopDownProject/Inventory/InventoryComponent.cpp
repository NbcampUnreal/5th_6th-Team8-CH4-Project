// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemData/ItemData.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h" 
#include "GameFramework/Actor.h"
#include "Inventory/UI/InventoryUI.h"
#include "Inventory/ItemData/ItemData.h"
#include "Inventory/ItemData/BaseItemComponent.h"

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
			InventoryWidget = CreateWidget<UInventoryUI>(PlayerController, InventoryWidgetClass);
			if (InventoryWidget) {
				InventoryWidget->OwnerInventoryComponent = this;
				InventoryWidget->AddToViewport();
			}
		}
	}
	
}

AActor* UInventoryComponent::SpawnItemOnGround(TSubclassOf<AActor> SpawnActor)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FVector Start = OwnerActor->GetActorLocation() + FVector(0.f, 0.f, 10.f);
	FVector Forward = OwnerActor->GetActorForwardVector();
	FVector End = Start + Forward * 300.f; 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	FHitResult ForwardHit;
	bool bHitForward = World->LineTraceSingleByChannel(
		ForwardHit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	FVector DownStart;
	if (bHitForward)
	{
		DownStart = ForwardHit.Location + FVector(0.f, 0.f, 50.f);
	}
	else
	{
		DownStart = End + FVector(0.f, 0.f, 50.f);
	}

	FVector DownEnd = DownStart - FVector(0.f, 0.f, 800.f);

	FHitResult DownHit;
	bool bHitDown = World->LineTraceSingleByChannel(
		DownHit,
		DownStart,
		DownEnd,
		ECC_Visibility,
		Params
	);

	if (!bHitDown)
	{
		return nullptr;
	}

	FVector SpawnLocation = DownHit.Location;
	FRotator SpawnRotation = OwnerActor->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;

	AActor* SpawnedItem = World->SpawnActor<AActor>(
		SpawnActor,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	return SpawnedItem;
}

bool UInventoryComponent::GetItem(AActor* ItemActor)
{
	if (UBaseItemComponent* ItemComp = ItemActor->FindComponentByClass<UBaseItemComponent>()) {
		AddItem(ItemComp->GetItemData());
		return true;
	}
	return false;
}

void UInventoryComponent::AddItem(FInventorySlot Item)
{
	if (Item.ItemID == NAME_None)
	{
		return;
	}

	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].ItemID == Item.ItemID)
		{
			
			const FItemData* ItemRow = ItemDataTable->FindRow<FItemData>(Item.ItemID, TEXT(""));
			if (!ItemRow) {
				UE_LOG(LogTemp, Warning, TEXT("ItemID not found in DataTable: %s"), *Item.ItemID.ToString());
				return;
			}
			Items[i].Num += Item.Num;

			bool IsRemain = false;
			if (ItemRow->MaxNum < Items[i].Num) {
				Item.Num = Items[i].Num - ItemRow->MaxNum;
				Items[i].Num = ItemRow->MaxNum;
				IsRemain = true;
			}
			
			if (GEngine){
				FString Msg = FString::Printf(
					TEXT("Stack Item [ %s ] -> Num : %d"),
					*Item.ItemID.ToString(),
					Items[i].Num
				);				
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
			}
			if (IsRemain) break;
			else return;
		}
	}

	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].ItemID == NAME_None)
		{
			Items[i] = Item;

			if (GEngine){
				FString Msg = FString::Printf(
					TEXT("New Item [ %s ] stored in Slot %d"),
					*Item.ItemID.ToString(),
					i
				);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
			}

			return;
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Inventory is Full"));
	}
}

void UInventoryComponent::DropItem(int32 Index)
{
	if (!Items.IsValidIndex(Index)) {
		UE_LOG(LogTemp, Warning, TEXT("InventoryIndex is not valid"));
		return;
	}	

	FName ItemID = Items[Index].ItemID;

	UDataTable* itemdatatable = GetDataTableByItemType(Items[Index].ItemType);

	if (!itemdatatable)	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTable is NULL"));
		return;
	}

	const FItemData* ItemRow = ItemDataTable->FindRow<FItemData>(ItemID, TEXT(""));
	if (!ItemRow)	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID not found in DataTable: %s"), *ItemID.ToString());
		return;
	}
	if (!ItemRow->ItemActorClass)	{
		UE_LOG(LogTemp, Warning, TEXT("ItemActorClass is NULL for item: %s"), *ItemID.ToString());
		return;
	}

	AActor* DroppedItem = SpawnItemOnGround(ItemRow->ItemActorClass);
	UBaseItemComponent* ItemComp = DroppedItem->FindComponentByClass<UBaseItemComponent>();
	if (!ItemComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no BaseItemComponent"), *DroppedItem->GetName());
		return;
	}
	ItemComp->ItemID = Items[Index].ItemID;
	ItemComp->ItemType = Items[Index].ItemType;
	//temp
	ItemComp->Num = Items[Index].Num;

	if (!DroppedItem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn dropped item"));
		return;
	}
	Items[Index].ItemID = "";
	UE_LOG(LogTemp, Log, TEXT("Dropped item: %s"), *ItemID.ToString());
}

void UInventoryComponent::RemoveItem(int32 Index) {

}

int32 UInventoryComponent::GetInventorytSize()
{
	if (ItemDataTable == nullptr)
	{
		return DefaultInventorySize;
	}

	if (EquipmentBagID.ItemID.IsNone())
	{
		return DefaultInventorySize;
	}

	const FBagItemData* ItemRow = ItemDataTable->FindRow<FBagItemData>(EquipmentBagID.ItemID, TEXT("ItemID"));

	if (ItemRow == nullptr)
	{
		return DefaultInventorySize;
	}

	return ItemRow->ContainerSize + DefaultInventorySize;
}

void UInventoryComponent::Open_CloseInventoryUI()
{

}

UDataTable* UInventoryComponent::GetDataTableByItemType(EItemType ItemType) const
{
	switch (ItemType)
	{
	case EItemType::Bag:
		return BagItemDataTable;

	case EItemType::Consumable:
		return ConsumableItemDataTable;

	case EItemType::Equipment_Head:
	case EItemType::Equipment_Body:
		return EquipmentItemDataTable;

	case EItemType::Ammo:
		return AmmoItemDataTable;

	default:
		return nullptr;
	}
}

void UInventoryComponent::SetEquipmentBagID(FInventorySlot NewID) {
	int32 curInventorySize = GetInventorytSize();
	EquipmentBagID = NewID; 
	int32 nextInventorySize = GetInventorytSize();
	if (curInventorySize > nextInventorySize) {
		for (int32 i = curInventorySize - 1; i >= nextInventorySize; --i)
		{
			DropItem(i);
		}
	}
	Items.SetNum(nextInventorySize);
}

int32 UInventoryComponent::GetBonusHealth()
{
	int32 BonusHealth =
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentChestID.ItemID, TEXT(""))->BonusHealth
		+ EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentHeadID.ItemID, TEXT(""))->BonusHealth;
	return BonusHealth;
}

int32 UInventoryComponent::GetDeffence()
{
	int32 Deffence =
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentChestID.ItemID, TEXT(""))->Deffence
		+ EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentHeadID.ItemID, TEXT(""))->Deffence;
	return Deffence;
}


//무기 장착
//EquipWeapon(GetEquipmentWeapon1ID()) or EquipWeapon(GetEquipmentWeapon2ID()) 
void UInventoryComponent::EquipWeapon(FInventorySlot NewWeapon)
{

	TSubclassOf<AActor> NewWeaponClass = 
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(NewWeapon.ItemID, TEXT(""))->ItemActorClass;
	if (!NewWeaponClass) return;

	// 기존 무기 제거
	UnequipWeapon();

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());

	EquippedWeaponActor = World->SpawnActor<AActor>(NewWeaponClass, Params);

	// 소켓 부착
	EquippedWeaponActor->AttachToComponent(
		GetOwner()->GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("WeaponSocket")
	);
}

void UInventoryComponent::UnequipWeapon()
{
	if (EquippedWeaponActor)
	{
		EquippedWeaponActor->Destroy();
		EquippedWeaponActor = nullptr;
	}
}

