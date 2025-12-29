// Weapons/WeaponBase.h
#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/WorldItemBase.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "WeaponBase.generated.h"

class UStaticMeshComponent;
class UNiagaraSystem;
class USoundBase;
class UBaseItemComponent;

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
    None,
    Range,
    Melee
};

USTRUCT(BlueprintType)
struct FWeaponCommonStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
    float Damage = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
    float AttackInterval = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
    bool bAutoRepeat = true;
};

UCLASS()
class CH4TOPDOWNPROJECT_API AWeaponBase : public AWorldItemBase
{
    GENERATED_BODY()

public:
    AWeaponBase();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    void StartAttack(const FVector_NetQuantize& TargetWorldPos);
    void StopAttack();

    UFUNCTION(Server, Unreliable)
    void Server_UpdateAim(const FVector_NetQuantize& NewTargetWorldPos);

    virtual void StartReload() {}

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UStaticMeshComponent> WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    EWeaponTypes WeaponType = EWeaponTypes::Range;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    FWeaponCommonStats CommonStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
    TObjectPtr<UNiagaraSystem> AttackFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|SFX")
    TObjectPtr<USoundBase> AttackSound;

    UFUNCTION(NetMulticast, Unreliable)
    virtual void Multicast_PlayAttackFX(const FVector& Loc, const FRotator& Rot);

protected:
    UPROPERTY()
    FVector_NetQuantize CachedTargetWorldPos;

    bool bWantsToAttack = false;
    float LastAttackTime = -FLT_MAX;
    FTimerHandle AttackTimerHandle;

protected:
    virtual float GetAttackInterval() const { return CommonStats.AttackInterval; }

    virtual bool Server_AttackOnce() PURE_VIRTUAL(AWeaponBase::Server_AttackOnce, return false;);

    void Server_AttackTick();

    bool CanAttack() const;

    virtual void Interact_Implementation(AActor* Interactor) override;
private:
    UFUNCTION(Server, Reliable)
    void Server_StartAttack(const FVector_NetQuantize& TargetWorldPos);

    UFUNCTION(Server, Reliable)
    void Server_StopAttack();
};
