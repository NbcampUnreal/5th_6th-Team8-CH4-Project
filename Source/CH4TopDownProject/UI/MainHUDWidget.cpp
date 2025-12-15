#include "UI/MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Component/HealthComponent.h"
#include "Component/StaminaComponent.h"

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
}

void UMainHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar && MaxHealth > 0)
    {
        HealthBar->SetPercent(FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f));
    }
}

void UMainHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
    if (StaminaBar && MaxStamina > 0)
    {
        StaminaBar->SetPercent(FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f));
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