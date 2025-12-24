// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TopDownWeaponBase.h"
#include "ShotgunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API AShotgunWeapon : public ATopDownWeaponBase
{
	GENERATED_BODY()
	
public:
	AShotgunWeapon();

protected:
	virtual void SpawnBullet_Server() override;

	virtual FVector ComputeBulletDirection_Server(const FVector& SpawnLoc) const override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 PelletsPerShot = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	float PelletDamageScale = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	float ExtraPelletSpreadDeg = 4.0f;
};
