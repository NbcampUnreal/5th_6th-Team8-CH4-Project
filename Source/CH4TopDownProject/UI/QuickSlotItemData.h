#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/ItemData.h"
#include "QuickSlotItemData.generated.h"

USTRUCT(BlueprintType)
struct FQuickSlotItemData
{
    GENERATED_BODY()

public:
    FQuickSlotItemData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int32 StackCount;
};