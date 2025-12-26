#include "Component/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/RCPlayerController.h"
#include "GameEvent/BlueZoneActor.h"
#include "Game/RCGameStateBase.h"

UHealthComponent::UHealthComponent()
    : CurrentHealth(100.0f)    
{    
    SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (GetOwner()->HasAuthority())
    {
        GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeDamage);

        CurrentHealth = MaxHealth;

        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    }
}

float UHealthComponent::GetCurrentHealth() const
{
    return CurrentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
    return MaxHealth;
}

float UHealthComponent::GetArmor() const
{
    return Armor;
}

void UHealthComponent::AddMaxHealth(float Amount)
{
    if (!GetOwner()->HasAuthority() || Amount == 0.0f)
    {
        return;
    }

    MaxHealth += Amount;
    CurrentHealth += Amount;

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::AddArmor(float Amount)
{
    if (!GetOwner()->HasAuthority() || Amount == 0.0f)
    {
        return;
    }

    Armor += Amount;
}

void UHealthComponent::Heal(float HealAmount)
{
    if (!GetOwner()->HasAuthority() || HealAmount <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHealthComponent, CurrentHealth);
    DOREPLIFETIME(UHealthComponent, MaxHealth);
    DOREPLIFETIME(UHealthComponent, Armor);
}

void UHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    ARCGameStateBase* RCGameState = GetWorld()->GetGameState<ARCGameStateBase>();
    if (IsValid(RCGameState) == false)
    {
        return;
    }

    if (RCGameState->MatchState != EMatchState::Playing)
    {
        return;
    }

    if (Damage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float ActualDamage = FMath::Max(Damage - Armor, 0.0f);

    float NewHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);

    if (DamageCauser && DamageCauser->IsA(ABlueZoneActor::StaticClass()))
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (OwnerPawn)
        {
            ARCPlayerController* PC = Cast<ARCPlayerController>(OwnerPawn->GetController());
            if (PC)
            {
                PC->Client_PlayBlueZoneHitEffect();
            }
        }
    }

    if (NewHealth <= 0.0f)
    {
        OnDeath.Broadcast();
        
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (!OwnerPawn)
        {
            return;
        }

        Multicast_SetDeadState();

        ARCPlayerController* PC = Cast<ARCPlayerController>(OwnerPawn->GetController());
        if (PC)
        {
            PC->Client_HandleDeath();
            PC->UnPossess();
        }

        if (OwnerPawn->HasAuthority())
        {
            PC->OnCharacterDead();

            GetWorld()->GetTimerManager().SetTimer(
                DestroyTimerHandle,
                [OwnerPawn]()
                {
                    OwnerPawn->Destroy();
                },
                3.0f,
                false
            );
        }
    }

    OnDamageReceived.Broadcast(ActualDamage, GetOwner()->GetActorLocation());
}

void UHealthComponent::Multicast_SetDeadState_Implementation()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn)
    {
        return;
    }

    OwnerPawn->SetActorHiddenInGame(true);
    OwnerPawn->SetActorEnableCollision(false);
}

void UHealthComponent::SetCurrentHealth(float NewHealth)
{ 
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    CurrentHealth = NewHealth;

    OnRep_Health();
}

void UHealthComponent::OnRep_Health()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}