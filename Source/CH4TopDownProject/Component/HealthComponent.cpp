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

    SetHealth(NewHealth);
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
    if (Damage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float ActualDamage = FMath::Max(Damage - Armor, 0.0f);

    float NewHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    SetHealth(NewHealth);

    if (NewHealth <= 0.0f)
    {
        OnDeath.Broadcast();
        
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (OwnerPawn)
        {
            OwnerPawn->SetActorHiddenInGame(true);
            OwnerPawn->SetActorEnableCollision(false);

            ARCPlayerController* PC = Cast<ARCPlayerController>(OwnerPawn->GetController());
            if (PC)
            {
                PC->Client_HandleDeath();
                PC->UnPossess();
            }

            if (OwnerPawn->HasAuthority())
            {
                OwnerPawn->Destroy();
            }
        }
    }

    OnDamageReceived.Broadcast(ActualDamage, GetOwner()->GetActorLocation());
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