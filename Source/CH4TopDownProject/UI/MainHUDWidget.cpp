#include "UI/MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Component/HealthComponent.h"
#include "Component/StaminaComponent.h"
#include "Component/QuickSlotComponent.h"
#include "UI/QuickSlotItemData.h"

void UMainHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APawn* OwningPawn = GetOwningPlayerPawn();
    if (!OwningPawn)
    {
        return;
    }

    PlayerHealthComponent = OwningPawn->FindComponentByClass<UHealthComponent>();
    if (!PlayerHealthComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not found HealthComp"));
        return;
    }

    PlayerHealthComponent->OnHealthChanged.AddDynamic(this, &UMainHUDWidget::UpdateHealth);

    UpdateHealth(PlayerHealthComponent->GetCurrentHealth(), PlayerHealthComponent->GetMaxHealth());

    PlayerStaminaComponent = OwningPawn->FindComponentByClass<UStaminaComponent>();
    if (!PlayerStaminaComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not found StaminaComp"));
        return;
    }

    PlayerStaminaComponent->OnStaminaChanged.AddDynamic(this, &UMainHUDWidget::UpdateStamina);

    UpdateStamina(PlayerStaminaComponent->GetCurrentStamina(), PlayerStaminaComponent->GetMaxStamina());


    PlayerQuickSlotComponent = OwningPawn->FindComponentByClass<UQuickSlotComponent>();
    if (!PlayerQuickSlotComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not found QuickSlotComp"));
        return;
    }

    PlayerQuickSlotComponent->OnQuickSlotDataChanged.AddDynamic(this, &UMainHUDWidget::UpdateQuickSlotData);
    
    UpdateQuickSlotData(PlayerQuickSlotComponent->GetQuickSlotData());
}

void UMainHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar && MaxHealth > 0)
    {
        HealthBar->SetPercent(FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f));
    }

    if (CurrentHealthText)
    {
        FString CurrentHealthString = FString::Printf(TEXT("%d"), FMath::RoundToInt(CurrentHealth));
        CurrentHealthText->SetText(FText::FromString(CurrentHealthString));
    }

    if (MaxHealthText)
    {
        FString MaxHealthString = FString::Printf(TEXT("%d"), FMath::RoundToInt(MaxHealth));
        MaxHealthText->SetText(FText::FromString(MaxHealthString));
    }
}

void UMainHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
    if (StaminaBar && MaxStamina > 0)
    {
        StaminaBar->SetPercent(FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f));
    }
}

void UMainHUDWidget::UpdateQuickSlotData(const TArray<FQuickSlotItemData>& NewSlotData)
{
    if (!QuickslotBox)
    {
        return;
    }

    int32 SlotCount = FMath::Min(QuickslotBox->GetChildrenCount(), NewSlotData.Num());
    
    for (int32 i = 0; i < SlotCount; ++i)
    {
        const FQuickSlotItemData& Data = NewSlotData[i];
        if (Data.ItemID != NAME_None)
        {
            UE_LOG(LogTemp, Display, TEXT("Slot %d: ItemID=%s, StackCount=%d"), i + 1, *Data.ItemID.ToString(), Data.StackCount);
        }
    }
}

void UMainHUDWidget::ShowNotice(const FString& Message)
{
    if (NoticeText)
    {
        NoticeText->SetText(FText::FromString(Message));        

        FTimerHandle NoticeTimer;
        GetWorld()->GetTimerManager().SetTimer(NoticeTimer, [this]()
            {
                if (NoticeText) NoticeText->SetVisibility(ESlateVisibility::Hidden);
            }, 5.0f, false);
    }
}

//void UMainHUDWidget::UpdateAmmoCount(int32 CurrentAmmo, int32 MaxAmmo)
//{
//    if (CurrentAmmoText)
//    {        
//        CurrentAmmoText->SetText(FText::AsNumber(CurrentAmmo));
//    }
//
//    if (MaxAmmoText)
//    {        
//        MaxAmmoText->SetText(FText::Format(NSLOCTEXT("HUD", "MaxAmmoFormat", "/ {0}"), FText::AsNumber(MaxAmmo)));
//    }
//}