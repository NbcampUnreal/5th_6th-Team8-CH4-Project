#include "UI/MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Component/HealthComponent.h"
#include "Component/StaminaComponent.h"
#include "Component/QuickSlotComponent.h"
#include "UI/QuickSlotItemData.h"
#include "Game/RCGameStateBase.h"
#include "Inventory/InventoryComponent.h"
#include "Components/Image.h"

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


    PlayerQuickSlotComponent = OwningPawn->GetComponentByClass<UInventoryComponent>()->GetQuickSlotComponent();
    if (!PlayerQuickSlotComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not found QuickSlotComp"));
        return;
    }

    PlayerQuickSlotComponent->OnQuickSlotDataChanged.AddDynamic(
        this, &UMainHUDWidget::UpdateQuickSlotData);
    PlayerQuickSlotComponent->OnQuickSlotDataChanged.Broadcast(PlayerQuickSlotComponent->GetQuickSlotData());
    //UpdateQuickSlotData(PlayerQuickSlotComponent->GetQuickSlotData());


    if (ARCGameStateBase* RCGameStateBase = GetWorld()->GetGameState<ARCGameStateBase>())
    {
        RCGameStateBase->OnAlivePlayersChanged.AddDynamic(this, &UMainHUDWidget::UpdateAlivePlayerCount);
        UpdateAlivePlayerCount(RCGameStateBase->AlivePlayerControllerCount);
        
        RCGameStateBase->OnReplicatedGameModeDelayChanged.AddDynamic(this, &UMainHUDWidget::ShowNoticeWithNoTimer);
        RCGameStateBase->OnShowNoti.AddDynamic(this, &UMainHUDWidget::ShowNotice);
        RCGameStateBase->OnFadeOut.AddDynamic(this, &UMainHUDWidget::FadeOut);
    }
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
    BP_UpdateQuickSlotData(NewSlotData);
}

void UMainHUDWidget::UpdateAmmoCount(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (CurrentAmmoText)
    {
        CurrentAmmoText->SetText(FText::AsNumber(CurrentAmmo));
    }

    if (MaxAmmoText)
    {
        MaxAmmoText->SetText(FText::AsNumber(MaxAmmo));
    }
}

void UMainHUDWidget::UpdateAlivePlayerCount(int32 AlivePlayerCount)
{
    if (AlivePlayerCountText)
    {
        FString AlivePlayerCountString = FString::Printf(TEXT("%d생존"), AlivePlayerCount);
        AlivePlayerCountText->SetText(FText::FromString(AlivePlayerCountString));
    }
}

void UMainHUDWidget::ShowNotice(const FString& Message)
{
    if (NoticeText)
    {
        if (NoticeText->GetVisibility() == ESlateVisibility::Hidden)
        {
            NoticeText->SetVisibility(ESlateVisibility::Visible);
        }

        NoticeText->SetText(FText::FromString(Message));        

        FTimerHandle NoticeTimer;
        GetWorld()->GetTimerManager().SetTimer(NoticeTimer, [this]()
            {
                if(NoticeText) NoticeText->SetVisibility(ESlateVisibility::Hidden);
            }, 5.0f, false);
    }
}

void UMainHUDWidget::ShowNoticeWithNoTimer(const FString& Message)
{
    if (NoticeText)
    {
        if (NoticeText->GetVisibility() == ESlateVisibility::Hidden)
        {
            NoticeText->SetVisibility(ESlateVisibility::Visible);
        }

        NoticeText->SetText(FText::FromString(Message));
    }
}

void UMainHUDWidget::FadeOut()
{
    if (FadeImg)
    {
        UFunction* PlayFadeOutAnim = FindFunction(FName("PlayFadeOutAnim"));
        if (PlayFadeOutAnim)
        {
            ProcessEvent(PlayFadeOutAnim, this);
        }
    }
}

void UMainHUDWidget::UpdateGameTime(float TimeInSeconds)
{
    if (TimerText)
    {
        TimeInSeconds = FMath::Max(0.0f, TimeInSeconds);

        int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
        int32 Seconds = FMath::FloorToInt(TimeInSeconds) % 60;

        FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        TimerText->SetText(FText::FromString(TimeString));
    }
}

void UMainHUDWidget::PlayBloodEffect()
{
    UFunction* PlayAnimFunc = FindFunction(FName("PlayBloodEffectAnim"));
    if (PlayAnimFunc)
    {
        ProcessEvent(PlayAnimFunc, this);
    }
}

void UMainHUDWidget::SetAmmoVisibility(ESlateVisibility InVisability)
{
    if (CurrentAmmoText)
    {
        CurrentAmmoText->SetVisibility(InVisability);
    }

    if (MaxAmmoText)
    {
        MaxAmmoText->SetVisibility(InVisability);
    }
}