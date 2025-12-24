// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RCGameStateBase.h"

#include "Net/UnrealNetwork.h"

void ARCGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
}
