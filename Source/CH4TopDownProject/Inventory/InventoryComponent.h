// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Inventory/ItemData/ItemData.h"
#include "InventoryComponent.generated.h"

class UInventoryUI;

struct FItemData;

USTRUCT(BlueprintType)
struct FInventorySlot // 아이템슬롯 == 아이템 한칸에 들어갈 정보들
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	int32 Num;
};


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma region UI
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UInventoryUI> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UInventoryUI> ContainerWidgetClass;

	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	UInventoryUI* InventoryWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	UInventoryUI* ContainerWidget;
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Open_CloseInventoryUI();
#pragma endregion

#pragma region Inventory
private: 
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> Items;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 DefaultInventorySize = 4; 	

	TMap<FName, int32> ItemCountCache;	

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* ItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* BagItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* ConsumableItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* EquipmentItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* WeqponItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* AmmoItemDataTable;
	
private:
	AActor* SpawnItemOnGround(TSubclassOf<AActor> SpawnActor);
	UDataTable* GetDataTableByItemType(EItemType ItemType)const;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GetItem(AActor* ItemActor);
	//아이템 획득, items에 추가와 cached에 등록
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FInventorySlot Item);
	//아이템 drop
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(FInventorySlot Item);
	//slot아이템 통째로 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(int32 Index);
	//사용한 아이템 갯수를 리턴
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 UseItem_ID(FName ItemID, int32 Num);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 CheckItem_ID(FName ItemID);


	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventorySlot>& GetItems() const { return Items; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetInventorytSize();
	
	
#pragma endregion

#pragma region Equipment

private:

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FInventorySlot EquipmentBagID;

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FInventorySlot EquipmentChestID;

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FInventorySlot EquipmentHeadID;
public:

	// Getter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FInventorySlot GetEquipmentBagID() const { return EquipmentBagID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FInventorySlot GetEquipmentChestID() const { return EquipmentChestID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FInventorySlot GetEquipmentHeadID() const { return EquipmentHeadID; }

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentBagID(FInventorySlot NewID);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentChestID(FInventorySlot NewID) { EquipmentChestID = NewID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentHeadID(FInventorySlot NewID) { EquipmentHeadID = NewID; }

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetBonusHealth();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetDeffence();
#pragma endregion

#pragma region Weapon

private:

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FInventorySlot EquipmentWeapon1ID;

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FInventorySlot EquipmentWeapon2ID;

	UPROPERTY(Transient)
	AActor* EquippedWeaponActor = nullptr;

public:

	// Getter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FInventorySlot GetEquipmentWeapon1ID() const { return EquipmentWeapon1ID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FInventorySlot GetEquipmentWeapon2ID() const { return EquipmentWeapon2ID; }

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentWeapon1(FInventorySlot NewID) { EquipmentWeapon1ID = NewID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentWeapon2(FInventorySlot NewID) { EquipmentWeapon2ID = NewID; }
	// Getter (Actor)
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AActor* GetWeaponActor() const { return EquippedWeaponActor; }


	// Equip
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipWeapon(FInventorySlot NewWeapon);
	// Unequip
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetUseableAmmoNum(FName UseAmmoID) const {
		return ItemCountCache[UseAmmoID];	}
#pragma endregion

};
