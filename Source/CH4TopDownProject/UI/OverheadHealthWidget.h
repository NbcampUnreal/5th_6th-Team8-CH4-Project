#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadHealthWidget.generated.h"

class UProgressBar;

UCLASS()
class CH4TOPDOWNPROJECT_API UOverheadHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;
};