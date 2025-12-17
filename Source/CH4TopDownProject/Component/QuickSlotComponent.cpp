#include "Component/QuickSlotComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "Inventory/ItemData/ItemData.h"
#include "UI/QuickSlotItemData.h"

UQuickSlotComponent::UQuickSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

const TArray<FQuickSlotItemData>& UQuickSlotComponent::GetQuickSlotData() const
{
    return QuickSlotData;
}

void UQuickSlotComponent::SetSlotItem(int32 SlotIndex, FName NewItemID, EItemType NewItemType, int32 NewStackCount)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (QuickSlotData.IsValidIndex(SlotIndex))
    {
        FQuickSlotItemData& Slot = QuickSlotData[SlotIndex];

        Slot.ItemID = NewItemID;
        Slot.ItemType = NewItemType;
        Slot.StackCount = NewStackCount;

        OnRep_QuickSlotData();
    }
}

void UQuickSlotComponent::Server_UseQuickSlot_Implementation(int32 SlotIndex)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (QuickSlotData.IsValidIndex(SlotIndex))
    {
        FQuickSlotItemData& Slot = QuickSlotData[SlotIndex];

        if (Slot.ItemID == NAME_None || Slot.ItemType == EItemType::None)
        {
            return;
        }

        if (Slot.ItemType == EItemType::Consumable && Slot.StackCount > 0)
        {
            --Slot.StackCount;

            if (Slot.StackCount <= 0)
            {
                Slot.ItemID = NAME_None;
                Slot.ItemType = EItemType::None;
            }

            OnRep_QuickSlotData();
        }
    }
}

bool UQuickSlotComponent::Server_UseQuickSlot_Validate(int32 SlotIndex)
{
    if (!QuickSlotData.IsValidIndex(SlotIndex))
    {
        return false;
    }

    const FQuickSlotItemData& Slot = QuickSlotData[SlotIndex];

    return Slot.ItemID != NAME_None && Slot.ItemType != EItemType::None;
}

void UQuickSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UQuickSlotComponent, QuickSlotData);
}

void UQuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		QuickSlotData.SetNum(NumQuickSlots);
	}
}

void UQuickSlotComponent::OnRep_QuickSlotData()
{
    OnQuickSlotDataChanged.Broadcast(QuickSlotData);
}