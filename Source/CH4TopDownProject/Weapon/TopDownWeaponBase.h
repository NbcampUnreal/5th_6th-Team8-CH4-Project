// TopDownWeaponBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopDownWeaponBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UNiagaraSystem;
class USoundBase;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Pistol,
    Rifle,
    Sniper,
    Shotgun,
};

USTRUCT(BlueprintType)
struct FWeaponStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireInterval = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Spread = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxRange = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MagazineSize = 30;

};


UCLASS()
class CH4TOPDOWNPROJECT_API ATopDownWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    ATopDownWeaponBase();

    virtual void BeginPlay() override;

    virtual void Fire();      
    virtual void StartFire();   
    virtual void StopFire();    

protected:


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    UStaticMeshComponent* MagazineMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* Muzzle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    EWeaponType WeaponType = EWeaponType::Rifle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    FWeaponStats WeaponStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
    UNiagaraSystem* MuzzleFlashFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
    USoundBase* FireSound;

    FTimerHandle FireTimerHandle;

    void PerformLineTrace();
    void PlayMuzzleFlash();
    void PlayFireSound();
};
