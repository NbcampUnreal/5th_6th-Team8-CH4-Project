// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RCGameStateBase.h"

#include "Net/UnrealNetwork.h"

void ARCGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, ReplicatedGameModeDelay);
	DOREPLIFETIME(ThisClass, bIsLoadedBattleLv);
}

void ARCGameStateBase::OnRep_AlivePlayerControllerCount()
{
	OnAlivePlayersChanged.Broadcast(AlivePlayerControllerCount);
}

void ARCGameStateBase::OnRep_ReplicatedGameModeDelay()
{
	FString NotiMsg;
	switch (MatchState)
	{
	case EMatchState::None:
		break;
	case EMatchState::Waiting:
		NotiMsg = FString::Printf(TEXT("게임 시작까지 %d초"), ReplicatedGameModeDelay);
		break;
	case EMatchState::Playing:
		NotiMsg = FString::Printf(TEXT("목표: 마지막까지 살아남으세요!"));
		break;
	case EMatchState::Ending:
		NotiMsg = FString::Printf(TEXT("게임 종료까지 %d초"), ReplicatedGameModeDelay);
		break;
	case EMatchState::Cleaning:
		break;
	default:
		break;
	}
	
	if (MatchState == EMatchState::Playing)
	{
		OnShowNoti.Broadcast(NotiMsg);
	}
	else
	{
		OnReplicatedGameModeDelayChanged.Broadcast(NotiMsg);
	}

	if (ReplicatedGameModeDelay <= 1 && bIsLoadedBattleLv == false && MatchState == EMatchState::Waiting)
	{
		bIsLoadedBattleLv = true;
		OnFadeOut.Broadcast();
	}
}
