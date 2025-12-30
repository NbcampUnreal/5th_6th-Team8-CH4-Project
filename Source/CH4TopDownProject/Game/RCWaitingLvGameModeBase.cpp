// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RCWaitingLvGameModeBase.h"

#include "Controller/RCPlayerController.h"
#include "Game/RCGameInstance.h"
#include "Game/RCGameStateBase.h"

void ARCWaitingLvGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitGameLv();
}

void ARCWaitingLvGameModeBase::InitGameLv()
{
	CurGameStateChangeDelay = GAME_START_DELAY;

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ARCWaitingLvGameModeBase::OnMainTimerElapsed()
{
	ARCGameStateBase* RCGameState = GetGameState<ARCGameStateBase>();
	if (IsValid(RCGameState) == false)
	{
		return;
	}

	RCGameState->AlivePlayerControllerCount = LogInPlayerControllers.Num();

	switch (RCGameState->MatchState)
	{
	case EMatchState::None: { break; }
	case EMatchState::Waiting:
	{
		if (LogInPlayerControllers.Num() >= MinPlayerCount)
		{
			UE_LOG(LogTemp, Error, TEXT("session is waiting.."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%d seconds until StartGame"), CurGameStateChangeDelay);
			RCGameState->ReplicatedGameModeDelay = CurGameStateChangeDelay;
			--CurGameStateChangeDelay;
		}

		if (CurGameStateChangeDelay < 0 && bIsLoadedToBattleLv == false)
		{
			bIsLoadedToBattleLv = true;
			GetWorld()->ServerTravel(TEXT("Battle_level"), true);
		}
		break;
	}
	case EMatchState::Playing: { break; }
	case EMatchState::Ending: { break; }
	case EMatchState::Cleaning: { break; }
	case EMatchState::End: { break; }
	default: { break; }
	}

}

void ARCWaitingLvGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ARCPlayerController* NewPlayerController = Cast<ARCPlayerController>(NewPlayer);
	if (NewPlayerController != nullptr)
	{
		LogInPlayerControllers.Add(NewPlayerController);

		if (LogInPlayerControllers.Num() >= MaxPlayerCount) {
			UE_LOG(LogTemp, Error, TEXT("Player is full. This Session will be closed..."));

			URCGameInstance* RCGameInstance = GetWorld()->GetGameInstance<URCGameInstance>();
			if (IsValid(RCGameInstance))
			{
				RCGameInstance->ManageSession(true);
			}
		}
	}
}

void ARCWaitingLvGameModeBase::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);

	ARCPlayerController* ExitingRCPlayerController = Cast<ARCPlayerController>(ExitingPlayer);
	if (IsValid(ExitingPlayer))
	{
		if (LogInPlayerControllers.Find(ExitingRCPlayerController) != INDEX_NONE)
		{
			LogInPlayerControllers.Remove(ExitingRCPlayerController);
		}
	}
}
