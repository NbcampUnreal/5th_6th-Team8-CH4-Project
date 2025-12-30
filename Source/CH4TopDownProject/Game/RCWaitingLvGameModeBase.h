// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RCWaitingLvGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API ARCWaitingLvGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	void InitGameLv();

#pragma region Settings

protected:
	const int32 GAME_START_DELAY = 10;
	int32 CurGameStateChangeDelay = 0;
	bool bIsLoadedToBattleLv = false;

private:
	UFUNCTION()
	void OnMainTimerElapsed();

public:
	FTimerHandle MainTimerHandle;


protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingPlayer) override;

	const int32 MinPlayerCount = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ARCPlayerController>> LogInPlayerControllers;
};
