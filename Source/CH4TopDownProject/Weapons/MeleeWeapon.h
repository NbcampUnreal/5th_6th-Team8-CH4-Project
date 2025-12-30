// Weapons/MeleeWeapon.h
#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "MeleeWeapon.generated.h"

class UNiagaraSystem;
class USceneComponent;
class USoundBase;

USTRUCT(BlueprintType)
struct FMeleeStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float Range = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float Radius = 35.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float AngleDeg = 90.f;
};

UCLASS()
class CH4TOPDOWNPROJECT_API AMeleeWeapon : public AWeaponBase
{
    GENERATED_BODY()

public:
    AMeleeWeapon();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    virtual bool Server_AttackOnce() override;
    virtual float GetAttackInterval() const override { return CommonStats.AttackInterval; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
    FMeleeStats MeleeStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee|VFX")
    TObjectPtr<UNiagaraSystem> SwingFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee|VFX")
    FName SwingSocketName = TEXT("BladeSocket");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee|SFX")
    TObjectPtr<USoundBase> SwingSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    TObjectPtr<USceneComponent> SwingPivot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    FRotator SwingRotA = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    FRotator SwingRotB = FRotator(-45.f, 0.f, -90.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    float SwingDuration = 0.3f;

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlaySwingFX();

private:
    bool IsInFrontArc(const FVector& OwnerForward, const FVector& ToTarget) const;

    void BeginSwingVisual();
    bool CanPlaySwingVisual() const;

    FRotator CachedPivotRot = FRotator::ZeroRotator;
    bool bSwinging = false;
    float SwingStartTime = 0.f;
    float LastSwingVisualTime = -FLT_MAX;
};
