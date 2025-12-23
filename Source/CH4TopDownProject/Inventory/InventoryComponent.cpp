// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemData/ItemData.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h" 
#include "GameFramework/Actor.h"
#include "Inventory/UI/InventoryUI.h"
#include "Inventory/ItemData/ItemData.h"
#include "Inventory/ItemData/BaseItemComponent.h"
#include "Character/RCPlayerCharacter.h"
#include "Component/HealthComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Items.SetNum(GetInventorytSize());
	WeaponActors.SetNum(2);

	Open_CloseInventoryUI();	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//inventory
	DOREPLIFETIME(UInventoryComponent, Items);
	DOREPLIFETIME(UInventoryComponent, ItemCountCache);
	//equipment
	DOREPLIFETIME(UInventoryComponent, EquipmentBagID);
	DOREPLIFETIME(UInventoryComponent, EquipmentChestID);
	DOREPLIFETIME(UInventoryComponent, EquipmentHeadID);
	//weapon
	DOREPLIFETIME(UInventoryComponent, WeaponActors);
	DOREPLIFETIME(UInventoryComponent, CurrentWeaponIndex);
}

void UInventoryComponent::Open_CloseInventoryUI()
{
	APlayerController* PlayerController =
		Cast<APlayerController>(GetOwner()->GetInstigatorController());

	if (!PlayerController)
		return;

	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
		return;
	}

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryUI>(PlayerController, InventoryWidgetClass);

		if (InventoryWidget)
		{
			InventoryWidget->OwnerInventoryComponent = this;
			InventoryWidget->AddToViewport();
		}
	}
}

#pragma region Inventory

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
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
		FInventorySlot Item = ItemComp->GetItemData();

		if (Item.ItemType == EItemType::Bag) {
			if (EquipmentBagID.ItemID == NAME_None) {
				SetEquipmentBagID(Item);
				return true;
			}
		}
		else if (Item.ItemType == EItemType::Equipment_Body) {
			if (EquipmentChestID.ItemID == NAME_None) {
				SetEquipmentChestID(Item);
				return true;
			}
		}
		else if (Item.ItemType == EItemType::Equipment_Head) {
			if (EquipmentHeadID.ItemID == NAME_None) {
				SetEquipmentHeadID(Item);
				return true;
			}
		}
		else if (Item.ItemType == EItemType::Weapon) {
			for (int i = 0; i < 2; i++) {
				if (!WeaponActors[i]) {
					RequestSetWeapon(i, Item);
				}
			}
		}
		else {
			AddItem(Item);
		}

		return true;
		}
	return false;
}

void UInventoryComponent::AddItem(FInventorySlot Item)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_AddItem(Item);
		return;
	}
	Server_AddItem_Implementation(Item);
}

void UInventoryComponent::Server_AddItem_Implementation(FInventorySlot Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;

	UDataTable* itemdatatable = GetDataTableByItemType(Item.ItemType);
	if (!itemdatatable)
		return;
	const FItemData* ItemRow = itemdatatable->FindRow<FItemData>(Item.ItemID, TEXT(""));
	if (!ItemRow) {
		//UE_LOG(LogTemp, Warning, TEXT("ItemID not found in DataTable: %s"), *Item.ItemID.ToString());
		return;
	}

	int32 RemainNum = Item.Num;

	for (int32 i = 0; i < Items.Num() && RemainNum > 0; ++i)
	{
		if (Items[i].ItemID == Item.ItemID)
		{
			int32 Addable = ItemRow->MaxNum - Items[i].Num;
			if (Addable <= 0) continue;

			int32 Added = FMath::Min(Addable, RemainNum);
			Items[i].Num += Added;
			RemainNum -= Added;
		}
	}

	for (int32 i = 0; i < Items.Num() && RemainNum > 0; ++i)
	{
		if (Items[i].ItemID == NAME_None)
		{
			int32 Added = FMath::Min(ItemRow->MaxNum, RemainNum);

			Items[i].ItemID = Item.ItemID;
			Items[i].ItemType = Item.ItemType;
			Items[i].Num = Added;

			RemainNum -= Added;
		}
	}

	int32 StoredNum = Item.Num - RemainNum;
	if (StoredNum > 0)
	{
		ItemCountCache.FindOrAdd(Item.ItemID) += StoredNum;
	}

	if (RemainNum > 0)
	{
		FInventorySlot DropSlot = Item;
		DropSlot.Num = RemainNum;

		DropItem_Internal(DropSlot);
	}
}

void UInventoryComponent::DropItem(FInventorySlot Item)
{	
	if (!GetOwner()->HasAuthority())
	{
		Server_DropItem(Item);
		return;
	}	
	DropItem_Internal(Item);
}

void UInventoryComponent::Server_DropItem_Implementation(FInventorySlot Item)
{
	DropItem_Internal(Item);
}

void UInventoryComponent::DropItem_Internal(FInventorySlot Item)
{
	FName ItemID = Item.ItemID;

	UDataTable* itemdatatable = GetDataTableByItemType(Item.ItemType);

	if (!itemdatatable) {
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTable is NULL"));
		return;
	}

	const FItemData* ItemRow = itemdatatable->FindRow<FItemData>(ItemID, TEXT(""));
	if (!ItemRow) {
		UE_LOG(LogTemp, Warning, TEXT("ItemID not found in DataTable: %s"), *ItemID.ToString());
		return;
	}
	if (!ItemRow->ItemActorClass) {
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
	ItemComp->ItemID = Item.ItemID;
	ItemComp->ItemType = Item.ItemType;
	//temp
	ItemComp->Num = Item.Num;

	if (!DroppedItem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn dropped item"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Dropped item: %s"), *ItemID.ToString());
}

void UInventoryComponent::RemoveItem(int32 Index) {
	if (!GetOwner()->HasAuthority())
	{
		Server_RemoveItem(Index);
	}

	Server_RemoveItem_Implementation(Index);
}

void UInventoryComponent::Server_RemoveItem_Implementation(int32 Index)
{
	const FName ItemID = Items[Index].ItemID;
	const int32 Num = Items[Index].Num;

	if (int32* Count = ItemCountCache.Find(ItemID))
	{
		*Count -= Num;

		if (*Count <= 0)
		{
			ItemCountCache.Remove(ItemID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID %s not found in ItemCountCache"), *ItemID.ToString());
	}

	Items[Index] = FInventorySlot();
}

int32 UInventoryComponent::UseItem_ID(FName ItemID, int32 Num)
{
	 if (!GetOwner()->HasAuthority())
	 {
		 Server_UseItem_ID(ItemID, Num);
		 return 0; // 클라는 즉시 결과를 모른다
	 }

	 UseItem_ID_Internal(ItemID, Num);
	 return 0;
}

void UInventoryComponent::Server_UseItem_ID_Implementation(FName ItemID, int32 Num)
{
	UseItem_ID_Internal(ItemID, Num);
}

void UInventoryComponent::UseItem_ID_Internal(FName ItemID, int32 Num)
{
	if (Num <= 0) return;

	int32 Remaining = Num;
	int32 Used = 0;

	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].ItemID != ItemID) continue;
		if (Remaining <= 0) break;

		const int32 UseNow = FMath::Min(Items[i].Num, Remaining);

		Items[i].Num -= UseNow;
		Remaining -= UseNow;
		Used += UseNow;

		if (int32* Count = ItemCountCache.Find(ItemID))
		{
			*Count -= UseNow;
			if (*Count <= 0)
			{
				ItemCountCache.Remove(ItemID);
			}
		}

		if (Items[i].Num == 0) {
			Items[i] = FInventorySlot();
		}
	}
	EItemType ItemType = GetDataTypeByItemID(ItemID);
	if (ItemType == EItemType::Consumable) {
		UDataTable* itemdatatable = GetDataTableByItemType(ItemType);
		const FConsumableItemData* ItemRow = itemdatatable->FindRow<FConsumableItemData>(ItemID, TEXT(""));
		for (int i = 0; i < Used; i++) {
			UHealthComponent* HealthComp =
				Cast<UHealthComponent>(GetOwner()->GetComponentByClass(UHealthComponent::StaticClass()));
			HealthComp->Heal(ItemRow->Heal);
		}
	}
	return;
}

int32 UInventoryComponent::CheckItem_ID(FName ItemID)
{
	if (ItemID.IsNone()) return 0;

	if (const int32* Count = ItemCountCache.Find(ItemID))
	{
		return *Count;
	}
	return 0;
}

int32 UInventoryComponent::GetInventorytSize()
{
	if (BagItemDataTable == nullptr)
	{
		return DefaultInventorySize;
	}

	if (EquipmentBagID.ItemID.IsNone())
	{
		return DefaultInventorySize;
	}

	const FBagItemData* ItemRow = BagItemDataTable->FindRow<FBagItemData>(EquipmentBagID.ItemID, TEXT("ItemID"));

	if (ItemRow == nullptr)
	{
		return DefaultInventorySize;
	}

	return ItemRow->ContainerSize + DefaultInventorySize;
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

EItemType UInventoryComponent::GetDataTypeByItemID(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return EItemType::None;
	}

	const FString ItemIDStr = ItemID.ToString();
	if (ItemIDStr.Len() == 0)
	{
		return EItemType::None;
	}

	const TCHAR FirstChar = ItemIDStr[0];

	if (!FChar::IsDigit(FirstChar))
	{
		return EItemType::None;
	}

	const int32 TypeIndex = FirstChar - '0';

	if (TypeIndex < 0 || TypeIndex > static_cast<int32>(EItemType::Weapon))
	{
		return EItemType::None;
	}

	return static_cast<EItemType>(TypeIndex);
}

#pragma endregion

#pragma region Equipment

void UInventoryComponent::OnRep_EquipmentBag()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::OnRep_EquipmentChest()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::OnRep_EquipmentHead()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SetEquipmentBagID(FInventorySlot NewID) {
	if (!GetOwner()->HasAuthority())
	{
		ServerSetEquipmentBagID(NewID);
		return;
	}

	ServerSetEquipmentBagID_Implementation(NewID);
}

void UInventoryComponent::SetEquipmentChestID(FInventorySlot NewID) { 
	if (!GetOwner()->HasAuthority())
	{
		ServerSetEquipmentChestID(NewID);
		return;
	}

	ServerSetEquipmentChestID_Implementation(NewID);
}

void UInventoryComponent::SetEquipmentHeadID(FInventorySlot NewID) { 
	if (!GetOwner()->HasAuthority())
	{
		ServerSetEquipmentHeadID(NewID);
		return;
	}

	ServerSetEquipmentHeadID_Implementation(NewID);
}

void UInventoryComponent::ServerSetEquipmentBagID_Implementation(FInventorySlot NewID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;

	int32 CurInventorySize = GetInventorytSize();
	EquipmentBagID = NewID;
	int32 NextInventorySize = GetInventorytSize();
	if (CurInventorySize > NextInventorySize) {
		for (int32 i = CurInventorySize - 1; i >= NextInventorySize; --i)
		{
			if (!Items.IsValidIndex(i))
				continue;

			DropItem_Internal(Items[i]);

			const FName ItemID = Items[i].ItemID;
			const int32 Num = Items[i].Num;

			if (int32* Count = ItemCountCache.Find(ItemID))
			{
				*Count -= Num;
				if (*Count <= 0)
				{
					ItemCountCache.Remove(ItemID);
				}
			}
		}
	}
	Items.SetNum(NextInventorySize);
}

void UInventoryComponent::ServerSetEquipmentChestID_Implementation(FInventorySlot NewID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
	EquipmentChestID = NewID;
}

void UInventoryComponent::ServerSetEquipmentHeadID_Implementation(FInventorySlot NewID)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
	EquipmentHeadID = NewID;
}

int32 UInventoryComponent::GetBonusHealth()
{	
	if (!EquipmentItemDataTable)
		return 0;

	int32 BonusHealth = 0;

	if (FEquipmentItemData* ItemData = 
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentChestID.ItemID, TEXT(""))) {
		BonusHealth += ItemData->BonusHealth;
	}
	if (FEquipmentItemData* ItemData =
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentHeadID.ItemID, TEXT(""))) {
		BonusHealth += ItemData->BonusHealth;
	}

	return BonusHealth;
}

int32 UInventoryComponent::GetDeffence()
{
	if (!EquipmentItemDataTable)
		return 0;

	int32 Deffence = 0;

	if (FEquipmentItemData* ItemData =
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentChestID.ItemID, TEXT(""))) {
		Deffence += ItemData->Deffence;
	}
	if (FEquipmentItemData* ItemData =
		EquipmentItemDataTable->FindRow<FEquipmentItemData>(EquipmentHeadID.ItemID, TEXT(""))) {
		Deffence += ItemData->Deffence;
	}

	return Deffence;
}

#pragma endregion

#pragma region Weapon

void UInventoryComponent::RequestSetWeapon(
	int32 Index, const FInventorySlot& NewWeapon)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerSetWeapon(Index, NewWeapon);
	}
}

void UInventoryComponent::RequestEquipWeapon(int32 Index)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipWeapon(Index);
	}
}

void UInventoryComponent::ServerSetWeapon_Implementation(int32 Index, FInventorySlot NewWeapon)
{
	if (!WeaponActors.IsValidIndex(Index)) return;
	if (!WeqponItemDataTable) return;

	const FEquipmentItemData* Row =
		WeqponItemDataTable->FindRow<FEquipmentItemData>(
			NewWeapon.ItemID, TEXT(""));

	if (!Row || !Row->ItemActorClass) return;

	// 기존 무기 제거
	ClearWeaponSlot(Index);

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());

	AActor* NewWeaponActor =
		GetWorld()->SpawnActor<AActor>(Row->ItemActorClass, Params);

	if (NewWeaponActor)
	{
		NewWeaponActor->SetReplicates(true);
		NewWeaponActor->SetActorHiddenInGame(true);
		WeaponActors[Index] = NewWeaponActor;
	}
}

void UInventoryComponent::ServerEquipWeapon_Implementation(int32 Index)
{
	if (Index != INDEX_NONE)
	{
		if (!WeaponActors.IsValidIndex(Index)) return;
		if (!WeaponActors[Index]) return;
	}

	CurrentWeaponIndex = Index;

	// 서버도 즉시 반영
	OnRep_CurrentWeaponIndex();
}

void UInventoryComponent::OnRep_CurrentWeaponIndex()
{
	for (AActor* Weapon : WeaponActors)
	{
		if (Weapon)
		{
			Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Weapon->SetActorHiddenInGame(true);
		}
	}

	if (!WeaponActors.IsValidIndex(CurrentWeaponIndex)) return;

	AActor* Weapon = WeaponActors[CurrentWeaponIndex];
	if (!Weapon) return;

	Weapon->SetActorHiddenInGame(false);
	Weapon->AttachToComponent(
		GetOwner()->GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("WeaponSocket")
	);
}

void UInventoryComponent::ClearWeaponSlot(int32 Index)
{
	if (!WeaponActors.IsValidIndex(Index)) return;

	AActor* Weapon = WeaponActors[Index];
	if (!Weapon) return;

	if (CurrentWeaponIndex == Index)
	{
		CurrentWeaponIndex = INDEX_NONE;
	}

	Weapon->Destroy();
	WeaponActors[Index] = nullptr;
}
#pragma endregion