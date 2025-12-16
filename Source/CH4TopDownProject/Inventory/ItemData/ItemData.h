#pragma once 

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemData.generated.h"

// class ABaseItem; // 아이템
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Bag,
	Consumable,
	Equipment_Head,
	Equipment_Body,
	Ammo,
}; 


USTRUCT(BlueprintType) 
struct FItemData : public FTableRowBase // 구조체가 `데이터 테이블의 한줄`이 되도록 만드는 역할
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
	int32 Num; //인벤토리에서 차지할 공간
	
};

USTRUCT(BlueprintType)
struct FBagItemData : public FTableRowBase
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
	int32 Capacity; //인벤토리 공간

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 MaxNum; //인벤토리에서 차지할 공간
};

USTRUCT(BlueprintType)
struct FConsumableItemData : public FTableRowBase
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
	int32 MaxNum; //인벤토리에서 차지할 공간

};

USTRUCT(BlueprintType)
struct FEquipmentItemData : public FTableRowBase
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
	int32 BonusHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 Deffence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 MaxNum; //인벤토리에서 차지할 공간

};

USTRUCT(BlueprintType)
struct FWeaponItemData : public FTableRowBase 
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
	int32 Capacity; //인벤토리에서 차지할 공간

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 ContainerSize; //인벤토리에서 차지할 공간
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 MaxNum; //인벤토리에서 차지할 공간

};

USTRUCT(BlueprintType)
struct FAmmoItemData : public FTableRowBase
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
	int32 Capacity; //인벤토리에서 차지할 공간

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 MaxNum; //인벤토리에서 차지할 공간

};