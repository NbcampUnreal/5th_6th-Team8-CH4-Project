// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot // 아이템슬롯 == 아이템 한칸에 들어갈 정보들
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	FName ItemID; // 아이템 DT_ItemData를 찾아갈 ID입니다
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	TSubclassOf<UUserWidget> InventoryWidgetClass;

public:

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	UUserWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
	UDataTable* ItemDataTable;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FName ItemID);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(FName ItemID);

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

	



};
