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

	void InitGame();

#pragma region Settings

protected:
	const int32 GameStateChangeDelay = 10;
	int32 CurGameStateChangeDelay = 10;

private:
	UFUNCTION()
	void OnMainTimerElapsed();

public:
	FTimerHandle MainTimerHandle;

#pragma endregion

#pragma region Player

protected:
	const int32 MaxPlayerCount = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ARCPlayerController>> AlivePlayerControllers;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ARCPlayerController>> DeadPlayerControllers;


protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingPlayer) override;
public:
	void OnPlayerDeath(ARCPlayerController* Controller);
#pragma endregion

#pragma region Object pooling
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Pistol_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Rifle_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Sniper_Class;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<AActor> BP_Bullet_Shotgun_Class;

protected:
	void InitPool();
#pragma endregion
};
