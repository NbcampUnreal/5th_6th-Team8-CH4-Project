#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UHorizontalBox;
class UImage;
class UHealthComponent;
class UStaminaComponent;
class UQuickSlotComponent;
struct FQuickSlotItemData;

class UMinimapWidget;

UCLASS()
class CH4TOPDOWNPROJECT_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStamina(float CurrentStamina, float MaxStamina);

    UFUNCTION()
    void UpdateQuickSlotData(const TArray<FQuickSlotItemData>& NewSlotData);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void BP_UpdateQuickSlotData(const TArray<FQuickSlotItemData>& NewSlotData);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAlivePlayerCount(int32 AlivePlayerCount);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNotice(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNoticeWithNoTimer(const FString& Message);
    FTimerHandle NotiVisibilityControlHandle;

    UFUNCTION()
    void FadeOut();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateGameTime(float TimeInSeconds);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void PlayBloodEffect();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateSlotAmmo(int32 SlotIndex, int32 CurrentAmmo, int32 TotalAmmo);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideAllAmmoUI();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowReloadProgress(float Duration);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void BP_ShowReloadProgress(float Duration);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowCrosshair();
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void BP_ShowCrosshair();
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentHealthText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxHealthText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> StaminaBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UMinimapWidget> MinimapWidget;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> QuickslotBox;

    UPROPERTY(meta = (Bindwidget))
    TObjectPtr<UTextBlock> NoticeText;

    UPROPERTY(meta = (Bindwidget))
    TObjectPtr<UTextBlock> TimerText;
    
    UPROPERTY(meta = (Bindwidget))
    TObjectPtr<UTextBlock> AlivePlayerCountText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Slot1_CurrentAmmo;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Slot1_MaxAmmo;
        
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Slot2_CurrentAmmo;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Slot2_MaxAmmo;
        
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> Slot1_Group;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> Slot2_Group;

    UPROPERTY()
    TObjectPtr<UHealthComponent> PlayerHealthComponent;

    UPROPERTY()
    TObjectPtr<UStaminaComponent> PlayerStaminaComponent;

    UPROPERTY()
    TObjectPtr<UQuickSlotComponent> PlayerQuickSlotComponent;    

    UPROPERTY(meta = (BindWidget))
    class UImage* FadeImg;
};