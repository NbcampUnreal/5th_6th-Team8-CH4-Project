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
#pragma region Inventory
private: 
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> Items;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 DefaultInventorySize = 4; 

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UInventoryUI> InventoryWidgetClass;

public:

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	UInventoryUI* InventoryWidget;

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
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FInventorySlot Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventorySlot>& GetItems() const { return Items; }

	int32 GetInventorytSize();
	
#pragma endregion


#pragma region Equipment

private:

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FName EquipmentBagID = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FName EquipmentChestID = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FName EquipmentHeadID = NAME_None;
public:

	// Getter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FName GetEquipmentBagID() const { return EquipmentBagID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FName GetEquipmentChestID() const { return EquipmentChestID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FName GetEquipmentHeadID() const { return EquipmentHeadID; }

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentBagID(FName NewID);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentChestID(FName NewID) { EquipmentChestID = NewID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentHeadID(FName NewID) { EquipmentHeadID = NewID; }
#pragma endregion

#pragma region Weapon

private:

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FName EquipmentWeapon1ID = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Equpment")
	FName EquipmentWeapon2ID = NAME_None;

public:

	// Getter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FName GetEquipmentWeapon1ID() const { return EquipmentWeapon1ID; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FName GetEquipmentWeapon2ID() const { return EquipmentWeapon2ID; }

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentWeapon1ID(FName NewID) {}
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentWeapon2ID(FName NewID) { EquipmentWeapon2ID = NewID; }
#pragma endregion
	



};
