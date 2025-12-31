// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Inventory/ItemData/ItemData.h"
#include "InventoryComponent.generated.h"

class UInventoryUI;
class UContainerWidget;
class AChest;
class UQuickSlotComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

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
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
#pragma region UI
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UInventoryUI> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UContainerWidget> ContainerWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UQuickSlotComponent> QuickSlotComponent;

public:
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Event")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	UInventoryUI* InventoryWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	UContainerWidget* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	bool IsInventoryOpen = false;
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventoryUI();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenChestUI(AChest* Chest);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventoryUI();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseChestUI();
	UFUNCTION(BlueprintPure, Category = "Components")
	UQuickSlotComponent* GetQuickSlotComponent() const;

#pragma endregion

#pragma region Inventory
private: 
	UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> Items;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 DefaultInventorySize = 4; 	

	
	TMap<FName, int32> ItemCountCache;
	//지금은 이렇게 하지만 tmap은 replicated에 사용하지 않는게 좋다. 데이터 꼬인다
protected:
	UFUNCTION()
	void OnRep_Items();
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
	UDataTable* WeaponItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* AmmoItemDataTable;
	
	UDataTable* GetDataTableByItemType(EItemType ItemType)const;

private:
	AActor* SpawnItemOnGround(TSubclassOf<AActor> SpawnActor);	
	EItemType GetDataTypeByItemID(FName ItemID)const;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GetItem(AActor* ItemActor);
	//아이템 획득, items에 추가와 cached에 등록
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FInventorySlot Item);
	UFUNCTION(Server, Reliable)
	void Server_AddItem(FInventorySlot Item);
	//아이템 drop
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(FInventorySlot Item);
	UFUNCTION(Server, Reliable)
	void Server_DropItem(FInventorySlot Item);
	void DropItem_Internal(FInventorySlot Item);
	//slot아이템 통째로 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(int32 Index);
	UFUNCTION(Server, Reliable)
	void Server_RemoveItem(int32 Index);
	void RemoveItem_Iternal(int32 Index);
	//사용한 아이템 갯수를 리턴
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 UseItem_ID(FName ItemID, int32 Num);
	UFUNCTION(Server, Reliable)
	void Server_UseItem_ID(FName ItemID, int32 Num);
	void UseItem_ID_Internal(FName ItemID, int32 Num);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 CheckItem_ID(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetTotalItemCountByID(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventorySlot>& GetItems() const { return Items; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetInventorytSize();
	
	
#pragma endregion

#pragma region Equipment

private:

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentBag, EditDefaultsOnly, Category = "Equipment")
	FInventorySlot EquipmentBagID;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentChest, EditDefaultsOnly, Category = "Equipment")
	FInventorySlot EquipmentChestID;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentHead, EditDefaultsOnly, Category = "Equipment")
	FInventorySlot EquipmentHeadID;
protected:
		UFUNCTION()
		void OnRep_EquipmentBag();
		UFUNCTION()
		void OnRep_EquipmentChest();
		UFUNCTION()
		void OnRep_EquipmentHead();
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
	void SetEquipmentChestID(FInventorySlot NewID);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipmentHeadID(FInventorySlot NewID);

	//ServerSetter
	UFUNCTION(Server, Reliable)
	void ServerSetEquipmentBagID(FInventorySlot NewID);
	UFUNCTION(Server, Reliable)
	void ServerSetEquipmentChestID(FInventorySlot NewID);
	UFUNCTION(Server, Reliable)
	void ServerSetEquipmentHeadID(FInventorySlot NewID);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetBonusHealth();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetDeffence();

	void HandleEquipmentHeadChanged();
	void HandleEquipmentChestChanged();
	
#pragma endregion

#pragma region Weapon

private:

	// 실제 장착된 무기 액터
	UPROPERTY(ReplicatedUsing = OnRep_WeaponActors)
	TArray<AActor*> WeaponActors; // Size = 2

	// 현재 사용 중인 무기 슬롯 인덱스
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponIndex)
	int32 CurrentWeaponIndex = 0;

protected:
	UFUNCTION()
	void OnRep_CurrentWeaponIndex();
	UFUNCTION()
	void OnRep_WeaponActors();
public:

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RequestSetWeapon(int32 Index, const FInventorySlot& NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RequestEquipWeapon(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerSetWeapon(int32 Index, FInventorySlot NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(int32 Index);

	void ClearWeaponSlot(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AActor* Get_CurrentWeapon();
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FInventorySlot Get_Weapon (int32 index) const;
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }
#pragma endregion

	

};
