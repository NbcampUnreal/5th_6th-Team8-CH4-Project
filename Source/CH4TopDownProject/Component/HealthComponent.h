#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangeSignature, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

protected:
	virtual void BeginPlay() override;    

    UFUNCTION()
    void OnRep_CurrentHealth(float OldHealth);

    UFUNCTION()
    void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    void SetHealth(float NewHealth);

public:    
    UPROPERTY(BlueprintAssignable)
    FOnHealthChangeSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnDeathSignature OnDeath;

    UFUNCTION(BlueprintPure)
    float GetCurrentHealth() const;

    UFUNCTION(BlueprintPure)
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure)
    float GetArmor() const;

    UFUNCTION(BlueprintCallable)
    void AddMaxHealth(float Amount);

    UFUNCTION(BlueprintCallable)
    void AddArmor(float Amount);

    UFUNCTION(BlueprintCallable)
    void Heal(float HealAmount);
        
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, Category = "Health")
    float CurrentHealth;

    UPROPERTY(Replicated, EditAnywhere, Category = "Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(Replicated, EditAnywhere, Category = "Armor", meta = (ClampMin = "0.0"))
    float Armor = 0.0f;
};