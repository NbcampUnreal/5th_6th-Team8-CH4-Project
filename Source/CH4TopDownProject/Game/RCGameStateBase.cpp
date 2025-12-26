// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RCGameStateBase.h"

#include "Net/UnrealNetwork.h"

void ARCGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, ReplicatedGameModeDelay);
}

void ARCGameStateBase::OnRep_AlivePlayerControllerCount()
{
	UE_LOG(LogTemp, Error, TEXT("AlivePlayerControllerCount :%d"), AlivePlayerControllerCount);
	OnAlivePlayersChanged.Broadcast(AlivePlayerControllerCount);
}
