#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UHorizontalBox;
class UImage;
class UHealthComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealthAndStamina(float CurrentHealth, float MaxHealth, float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStamina(float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmoCount(int32 CurrentAmmo, int32 MaxClipAmmo);

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> StaminaBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentAmmoText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxAmmoText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> QuickslotBox;

    UPROPERTY()
    TObjectPtr<UHealthComponent> PlayerHealthComponent;
};