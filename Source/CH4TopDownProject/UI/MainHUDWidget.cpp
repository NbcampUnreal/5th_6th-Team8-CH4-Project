#include "UI/MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMainHUDWidget::UpdateHealthAndStamina(float CurrentHealth, float MaxHealth, float CurrentStamina, float MaxStamina)
{
    if (HealthBar && MaxHealth > 0)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
        
    if (StaminaBar && MaxStamina > 0)
    {
        StaminaBar->SetPercent(CurrentStamina / MaxStamina);
    }
}

void UMainHUDWidget::UpdateAmmoCount(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (CurrentAmmoText)
    {        
        CurrentAmmoText->SetText(FText::AsNumber(CurrentAmmo));
    }

    if (MaxAmmoText)
    {        
        MaxAmmoText->SetText(FText::Format(NSLOCTEXT("HUD", "MaxAmmoFormat", "/ {0}"), FText::AsNumber(MaxAmmo)));
    }
}

void UMainHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();


}