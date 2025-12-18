#include "Component/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/RCPlayerController.h"

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

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

    SetHealth(NewHealth);

    if (NewHealth <= 0.0f)
    {        
        OnDeath.Broadcast();

        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (OwnerPawn)
        {
            ARCPlayerController* PC = Cast<ARCPlayerController>(OwnerPawn->GetController());
            if (PC)
            {
                PC->Client_HandleDeath();
            }
        }
    }
}

void UHealthComponent::SetHealth(float NewHealth)
{    
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    float OldHealth = CurrentHealth;
    CurrentHealth = NewHealth;

    OnRep_CurrentHealth(OldHealth);
}

void UHealthComponent::OnRep_CurrentHealth(float OldHealth)
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}