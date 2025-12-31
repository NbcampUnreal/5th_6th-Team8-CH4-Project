// Weapons/RangeWeapon.h
#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "RangeWeapon.generated.h"

class USceneComponent;
class ABulletBase;
class UInventoryComponent;

USTRUCT(BlueprintType)
struct FRangeWeaponStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
    float BulletSpeed = 3000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
    float SpreadDeg = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
    float MaxRange = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
    int32 MagazineSize = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
    float ReloadDuration = 1.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
    float MuzzleOffset = 30.f;
};

UCLASS()
class CH4TOPDOWNPROJECT_API ARangeWeapon : public AWeaponBase
{
    GENERATED_BODY()

public:
    ARangeWeapon();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
    TObjectPtr<UStaticMeshComponent> MagazineMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
    TObjectPtr<USceneComponent> Muzzle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range")
    FRangeWeaponStats RangeStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
    TSubclassOf<ABulletBase> BulletClass;

    UPROPERTY(ReplicatedUsing = OnRep_Reloading, BlueprintReadOnly, Category = "Range")
    bool bIsReloading = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range|Ammo")
    FName AmmoItemID = TEXT("4_Ammo_Default");

    UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Range")
    int32 CurrentAmmo = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range|SFX")
    TObjectPtr<USoundBase> ReloadSound;

    FTimerHandle ReloadTimerHandle;

    int32 PendingReloadFill = 0;

    bool bReloadRequested = false;

    bool bPrevReloading = false;
protected:
    virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual bool Server_AttackOnce() override;
    virtual float GetAttackInterval() const override;


public:
    virtual void StartReload() override;

    UFUNCTION(BlueprintCallable)
    FName GetAmmoItemID() const { return AmmoItemID; }

    int32 GetCurrentAmmo() const { return CurrentAmmo; }

    int32 GetMaxMagazineSize() const { return RangeStats.MagazineSize; }

private:
    UFUNCTION(Server, Reliable)
    void Server_StartReload();

    bool StartReload_Internal();

    bool CanShoot() const;

    UInventoryComponent* GetOwnerInventory_Server() const;
    int32 GetMissingAmmoToFill() const;

    void SpawnBullet_Server();
    FVector ComputeBulletDirection_Server(const FVector& SpawnLoc) const;

    void FinishReload_Server();

    UFUNCTION()
    void OnRep_Ammo();

    UFUNCTION()
    void OnRep_Reloading();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
