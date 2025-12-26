#pragma once

#include "CoreMinimal.h"
#include "Weapon/TopDownWeaponBase.h"
#include "MeleeWeapon.generated.h"

USTRUCT(BlueprintType)
struct FMeleeStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float Damage = 35.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float Range = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float AttackInterval = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float Radius = 35.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float AngleDeg = 90.f;
};

UCLASS()
class CH4TOPDOWNPROJECT_API AMeleeWeapon : public ATopDownWeaponBase
{
    GENERATED_BODY()

public:
    AMeleeWeapon();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
    FMeleeStats MeleeStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee|VFX")
    TObjectPtr<UNiagaraSystem> SwingFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee|VFX")
    FName SwingSocketName = TEXT("BladeSocket");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    TObjectPtr<USceneComponent> SwingPivot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    FRotator SwingRotA = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    FRotator SwingRotB = FRotator(-45.f, 0.f, -90.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee|Swing")
    float SwingDuration = 0.3f;

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlaySwingFX();

protected:
    FRotator CachedPivotRot;

    bool bSwinging = false;

    float SwingStartTime = 0.f;

    float LastSwingVisualTime = -FLT_MAX;

    virtual void Tick(float DeltaSeconds) override;

    virtual void StartFire() override;

    void BeginSwingVisual();

    bool CanPlaySwingVisual() const;

    virtual void Server_AttackOnce() override;

    virtual float GetAttackInterval() const override;

    bool IsInFrontArc(const FVector& OwnerForward, const FVector& ToTarget) const;
};
