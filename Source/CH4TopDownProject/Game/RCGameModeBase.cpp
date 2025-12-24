
#include "RCGameModeBase.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Game/RCGameInstance.h"
#include "Game/RCGameStateBase.h"

#include "Controller/RCPlayerController.h"

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//InitGame();
	InitPool();
}

void ARCGameModeBase::InitGame()
{
	URCGameInstance* GI = GetWorld()->GetGameInstance<URCGameInstance>();
	if (GI)
	{
		GI->ManageSession(false);
	}

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ARCGameModeBase::OnMainTimerElapsed()
{
	ARCGameStateBase* RCGameState = GetGameState<ARCGameStateBase>();
	if (IsValid(RCGameState) == false)
	{
		return;
	}

	RCGameState->AlivePlayerControllerCount = AlivePlayerControllers.Num();

	switch (RCGameState->MatchState)
	{
	case EMatchState::None: { break; }
	case EMatchState::Waiting:
	{
		if (AlivePlayerControllers.Num() < MaxPlayerCount)
		{

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Ready To Game :%d"), CurGameStateChangeDelay);
			--CurGameStateChangeDelay;
		}

		if (CurGameStateChangeDelay <= 0)
		{
			RCGameState->MatchState = EMatchState::Playing;
		}
		break;
	}
	case EMatchState::Playing:
	{ 


		break; 
	}
	case EMatchState::Ending: 
	{ 
		--CurGameStateChangeDelay;
		if (CurGameStateChangeDelay <= 0)
		{
			FName CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(this));
			UGameplayStatics::OpenLevel(this, CurrentLevelName, true, FString(TEXT("listen")));
		}
		break; 
	}
	case EMatchState::End: { break; }
	default: { break; }
	}
}

void ARCGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ARCPlayerController* NewPlayerController = Cast<ARCPlayerController>(NewPlayer);
	if (NewPlayerController != nullptr)
	{
		AlivePlayerControllers.Add(NewPlayerController);

		if (AlivePlayerControllers.Num() >= MaxPlayerCount) {
			UE_LOG(LogTemp, Error, TEXT("Player is full. This Session will be closed..."));

			URCGameInstance* RCGameInstance = GetWorld()->GetGameInstance<URCGameInstance>();
			if (IsValid(RCGameInstance))
			{
				RCGameInstance->ManageSession(true);
			}
		}
	}
}

void ARCGameModeBase::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);

	ARCPlayerController* ExitingRCPlayerController = Cast<ARCPlayerController>(ExitingPlayer);
	if (IsValid(ExitingPlayer))
	{
		if (AlivePlayerControllers.Find(ExitingRCPlayerController) != INDEX_NONE)
		{
			AlivePlayerControllers.Remove(ExitingRCPlayerController);
		}
		else
		{
			DeadPlayerControllers.Remove(ExitingRCPlayerController);
		}
	}
}

void ARCGameModeBase::OnPlayerDeath(ARCPlayerController* Controller)
{
	UE_LOG(LogTemp, Error, TEXT("on player death"));

	AlivePlayerControllers.Remove(Controller);
	DeadPlayerControllers.Add(Controller);

	if (AlivePlayerControllers.Num() <= 1)
	{
		ARCGameStateBase* RCGameState = GetGameState<ARCGameStateBase>();
		if (IsValid(RCGameState))
		{
			RCGameState->MatchState = EMatchState::Ending;
			CurGameStateChangeDelay = GameStateChangeDelay;
		}
	}
}

void ARCGameModeBase::InitPool()
{
	if (UActorObjectPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorObjectPoolSubsystem>())
	{
		Pool->InitializePool(BP_Bullet_Pistol_Class, 200);
		Pool->InitializePool(BP_Bullet_Rifle_Class, 300);
		Pool->InitializePool(BP_Bullet_Sniper_Class, 150);
		Pool->InitializePool(BP_Bullet_Shotgun_Class, 250);
	}
}
