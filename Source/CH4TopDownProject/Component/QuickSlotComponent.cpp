#include "Component/QuickSlotComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "Inventory/ItemData/ItemData.h"
#include "UI/QuickSlotItemData.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/UI/ItemWidget.h"

UQuickSlotComponent::UQuickSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UQuickSlotComponent::BeginPlay()
{
    Super::BeginPlay();
    QuickSlotData.SetNum(NumQuickSlots);
}

const TArray<FQuickSlotItemData>& UQuickSlotComponent::GetQuickSlotData() const
{
    return QuickSlotData;
}

void UQuickSlotComponent::UseQuickSlot(int32 SlotIndex)
{
    if (SelectItemWidget) {
        UItemWidget* itemwidget = Cast<UItemWidget>(SelectItemWidget);
        Server_SetQuickSlot(SlotIndex,
            itemwidget->GetItemData().ItemID,
            itemwidget->GetItemData().ItemType
        );
        return;
    }
    else {
        Server_UseQuickSlot(SlotIndex);
        return;
    }
}

void UQuickSlotComponent::Server_SetQuickSlot(int32 SlotIndex, FName NewItemID, EItemType NewItemType)
{
    if (!QuickSlotData.IsValidIndex(SlotIndex))
    {
        return;
    }
    UInventoryComponent* Inventory = GetInventoryComponent();
    if (!Inventory)return;
    
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

    FQuickSlotItemData& Slot = QuickSlotData[SlotIndex];
    Slot.ItemID = NewItemID;
    Slot.ItemType = NewItemType;
    QuickSlotData = QuickSlotData;

    UE_LOG(LogTemp, Warning, TEXT("Broadcast Role: %s"),
        *UEnum::GetValueAsString(GetOwnerRole()));

    if (Inventory)
    {
        Slot.StackCount = Inventory->CheckItem_ID(NewItemID);
    }

    OnQuickSlotDataChanged.Broadcast(GetQuickSlotData());
    //OnRep_QuickSlotData();
}

void UQuickSlotComponent::Server_UseQuickSlot(int32 SlotIndex)
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
        Inventory->UseItem_ID(Slot.ItemID, 1);
    }


}

void UQuickSlotComponent::UpdateSlotCount(const TMap<FName, int32>& ItemCountCache)
{
    bool bUpdated = false;
    for (int i = 2; i < QuickSlotData.Num(); i++) {
        FQuickSlotItemData& Slot = QuickSlotData[i];
        if (const int32* Count = ItemCountCache.Find(Slot.ItemID))
        {
            if (*Count > 0)
            {
                Slot.StackCount = *Count;
                bUpdated = true;
            }
            
      
        }
        else {
            Slot.ItemID = "";
            Slot.StackCount = 0;
        }
        bUpdated = true;
    }

    if (bUpdated)
    {
        OnRep_QuickSlotData();
    }
}

void UQuickSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UQuickSlotComponent::SetQuickSlot(UUserWidget* widget)
{
    SelectItemWidget = widget;
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