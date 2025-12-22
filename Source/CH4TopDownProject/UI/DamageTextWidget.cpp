#include "UI/DamageTextWidget.h"
#include "Components/TextBlock.h"

void UDamageTextWidget::SetDamageText(float DamageAmount)
{
    if (DamageText)
    {
        FText DamageValue = FText::AsNumber(FMath::RoundToInt(DamageAmount));
        DamageText->SetText(DamageValue);
    }
}