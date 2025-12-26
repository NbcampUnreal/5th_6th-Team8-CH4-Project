// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RCGameStateBase.generated.h"


UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	Cleaning,
	End
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlivePlayersChanged, int32, NewAliveCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplicatedGameModeDelayChanged, const FString&, NotiMsg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowNoti, const FString&, NotiMsg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeOut);
/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API ARCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated)
	EMatchState MatchState = EMatchState::Waiting;

public:
	UPROPERTY(ReplicatedUsing = OnRep_AlivePlayerControllerCount)
	int32 AlivePlayerControllerCount = 0;

	UPROPERTY(BlueprintAssignable)
	FOnAlivePlayersChanged OnAlivePlayersChanged;
protected:
	UFUNCTION()
	void OnRep_AlivePlayerControllerCount();


public:
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedGameModeDelay)
	int32 ReplicatedGameModeDelay = 0;

	UPROPERTY(Replicated)
	bool bIsLoadedBattleLv = false;

	UPROPERTY(BlueprintAssignable)
	FOnReplicatedGameModeDelayChanged OnReplicatedGameModeDelayChanged;
	FOnFadeOut OnFadeOut;

	FOnShowNoti OnShowNoti;

protected:
	UFUNCTION()
	void OnRep_ReplicatedGameModeDelay();

};
