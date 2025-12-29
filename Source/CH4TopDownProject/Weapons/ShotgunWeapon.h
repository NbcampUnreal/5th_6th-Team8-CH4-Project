// Weapons/ShotgunWeapon.h
#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangeWeapon.h"
#include "ShotgunWeapon.generated.h"

UCLASS()
class CH4TOPDOWNPROJECT_API AShotgunWeapon : public ARangeWeapon
{
    GENERATED_BODY()

public:
    AShotgunWeapon();

protected:

    virtual bool Server_AttackOnce() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
    int32 PelletsPerShot = 8;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
    float PelletDamageScale = 1.0f / 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
    float ExtraPelletSpreadDeg = 4.0f;
};
