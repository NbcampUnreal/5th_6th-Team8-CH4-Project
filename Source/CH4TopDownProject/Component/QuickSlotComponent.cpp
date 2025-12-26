#include "Component/QuickSlotComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "Inventory/ItemData/ItemData.h"
#include "UI/QuickSlotItemData.h"
#include "Inventory/InventoryComponent.h"

UQuickSlotComponent::UQuickSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UQuickSlotComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        QuickSlotData.SetNum(NumQuickSlots);
    }
}

const TArray<FQuickSlotItemData>& UQuickSlotComponent::GetQuickSlotData() const
{
    return QuickSlotData;
}

void UQuickSlotComponent::Server_SetQuickSlot_Implementation(int32 SlotIndex, FName NewItemID, EItemType NewItemType)
{
    if (!QuickSlotData.IsValidIndex(SlotIndex))
    {
        return;
    }
    UInventoryComponent* Inventory = GetInventoryComponent();
    if (Inventory)return;
    FQuickSlotItemData& Slot = QuickSlotData[SlotIndex];
    Slot.ItemID = NewItemID;
    Slot.ItemType = NewItemType;

    if (Inventory)
    {
        Slot.StackCount = Inventory->CheckItem_ID(NewItemID);
    }
    // 1, 2 -> Weapon
    if (SlotIndex <= 1)
    {
        if (NewItemType != EItemType::Weapon && NewItemType != EItemType::None)
        {

            return;
        }
    }
    // 3 ~ 8 -> Consumable
    else
    {
        if (NewItemType != EItemType::Consumable && NewItemType != EItemType::None)
        {
            return;
        }
    }

    OnRep_QuickSlotData();
}

void UQuickSlotComponent::Server_UseQuickSlot_Implementation(int32 SlotIndex)
{
    if (!QuickSlotData.IsValidIndex(SlotIndex))
    {
        return;
    }

    FQuickSlotItemData& Slot = QuickSlotData[SlotIndex];
    if (Slot.ItemID == NAME_None || Slot.ItemType == EItemType::None)
    {
        return;
    }

    UInventoryComponent* Inventory = GetInventoryComponent();
    if (!Inventory)
    {
        return;
    }

    if (Slot.ItemType == EItemType::Weapon)
    {
        FInventorySlot WeaponSlot;
        WeaponSlot.ItemID = Slot.ItemID;
        WeaponSlot.ItemType = Slot.ItemType;

        Inventory->RequestEquipWeapon(SlotIndex);
    }
    else if (Slot.ItemType == EItemType::Consumable)
    {
        Inventory->UseItem_ID(Slot.ItemID,1);
    }
}

bool UQuickSlotComponent::Server_UseQuickSlot_Validate(int32 SlotIndex)
{
    return QuickSlotData.IsValidIndex(SlotIndex);
}

void UQuickSlotComponent::UpdateSlotCount(FName ItemID, int32 NewCount)
{
    bool bUpdated = false;
    for (FQuickSlotItemData& Slot : QuickSlotData)
    {
        if (Slot.ItemID == ItemID)
        {
            Slot.StackCount = NewCount;
            if (NewCount <= 0 && Slot.ItemType == EItemType::Consumable)
            {
                // Icon?
            }

            bUpdated = true;
        }
    }

    if (bUpdated)
    {
        OnRep_QuickSlotData();
    }
}

void UQuickSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UQuickSlotComponent, QuickSlotData);
}

UInventoryComponent* UQuickSlotComponent::GetInventoryComponent() const
{
    if (AActor* Owner = GetOwner())
    {
        return Owner->FindComponentByClass<UInventoryComponent>();
    }

    return nullptr;
}

void UQuickSlotComponent::OnRep_QuickSlotData()
{
    OnQuickSlotDataChanged.Broadcast(QuickSlotData);
}