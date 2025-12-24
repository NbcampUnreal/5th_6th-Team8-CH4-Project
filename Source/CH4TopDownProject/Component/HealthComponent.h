#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageReceivedSignature, float, DamageAmount, FVector, HitLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

protected:
	virtual void BeginPlay() override;    

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_Health();

    UFUNCTION()
    void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetDeadState();

    void SetCurrentHealth(float NewHealth);

public:    
    UPROPERTY(BlueprintAssignable)
    FOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnDamageReceivedSignature OnDamageReceived;

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

protected:
    UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Health")
    float CurrentHealth;

    UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(Replicated, EditAnywhere, Category = "Armor", meta = (ClampMin = "0.0"))
    float Armor = 0.0f;

    FTimerHandle DestroyTimerHandle;
};