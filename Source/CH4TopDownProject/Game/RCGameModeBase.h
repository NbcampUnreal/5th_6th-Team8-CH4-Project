// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RCGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API ARCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Pistol_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Rifle_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Sniper_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Shotgun_Class;
};
