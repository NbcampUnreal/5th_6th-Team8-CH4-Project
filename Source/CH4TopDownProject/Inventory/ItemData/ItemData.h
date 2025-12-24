#pragma once 

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemData.generated.h"

// class ABaseItem; // 아이템
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,//0
	Bag,//1
	Consumable,//2
	Equipment_Head,//3
	Equipment_Body,//4
	Ammo,//5
	Weapon//6
}; 


USTRUCT(BlueprintType) 
struct FItemData : public FTableRowBase 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FName ItemID; // 아이템ID Apple, Potion

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FText ItemName; // 유저가 보는 아이템 이름 사과, 체력포션

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	UTexture2D* Thumbnail; // 인벤토리에서 보여줄 아이템 썸네일

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TSubclassOf<AActor> ItemActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 MaxNum; //인벤토리 한칸의 크기
};

USTRUCT(BlueprintType)
struct FBagItemData : public FItemData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 ContainerSize; //인벤토리 공간
};

USTRUCT(BlueprintType)
struct FConsumableItemData : public FItemData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	float Heal; //회복
};

USTRUCT(BlueprintType)
struct FEquipmentItemData : public FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 BonusHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 Deffence;
};

USTRUCT(BlueprintType)
struct FWeaponItemData : public FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FName UseAmmoID;
};

USTRUCT(BlueprintType)
struct FAmmoItemData : public FItemData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FWeaponAmmoSet : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName WeaponID;

	UPROPERTY(EditAnywhere)
	FName AmmoID;
	
	UPROPERTY(EditAnywhere)
	float Weight;
};

USTRUCT(BlueprintType)
struct FWeightedItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ItemID;

	UPROPERTY(EditAnywhere)
	int32 MinCount;

	UPROPERTY(EditAnywhere)
	int32 MaxCount;

	UPROPERTY(EditAnywhere)
	float Weight;
};

USTRUCT(BlueprintType)
struct FWeightedItemArray : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWeightedItem> Items;
};

USTRUCT(BlueprintType)
struct FWeightedItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;
};
