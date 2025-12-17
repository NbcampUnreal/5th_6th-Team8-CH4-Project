// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/WorldItemBase.h"
#include "Barrel.generated.h"

class USphereComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API ABarrel : public AWorldItemBase
{
	GENERATED_BODY()
	
public:
	ABarrel();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int ExplosionDamage;

	FTimerHandle ExplosionTimerHandler;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void TakeDamage_Implementation(float Damage, AActor* DamageCauser) override;
};
