#include "UI/OverheadHealthWidget.h"
#include "Components/ProgressBar.h"

void UOverheadHealthWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (HealthBar && MaxHealth > 0)
    {
        HealthBar->SetPercent(FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f));
    }
}