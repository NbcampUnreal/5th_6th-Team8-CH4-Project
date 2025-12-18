// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/WorldItemBase.h"
#include "Barricade.generated.h"

UCLASS()
class CH4TOPDOWNPROJECT_API ABarricade : public AWorldItemBase
{
	GENERATED_BODY()
	
public:
	ABarricade();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* DeathParticle;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void TakeDamage_Implementation(float Damage, AActor* DamageCauser) override;
	

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();
};
