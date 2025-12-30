
#include "RCGameModeBase.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Game/RCGameInstance.h"
#include "Game/RCGameStateBase.h"

#include "Controller/RCPlayerController.h"
#include "Spawner/SpawnManager.h"

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitGameLv();
	InitPool();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnManager::StaticClass(), GetAllActorList);
	if (GetAllActorList.IsEmpty() == false )
	{
		SpawnManager = Cast<ASpawnManager>(GetAllActorList[0]);
	}
}

void ARCGameModeBase::InitGameLv()
{
	CurGameStateChangeDelay = GAME_START_DELAY;

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
			UE_LOG(LogTemp, Error, TEXT("session is waiting.."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%d seconds until StartGame"), CurGameStateChangeDelay);

			//RCGameState->ReplicatedGameModeDelay = CurGameStateChangeDelay;
			--CurGameStateChangeDelay;
		}

		if (CurGameStateChangeDelay < 0)
		{
			RCGameState->MatchState = EMatchState::Playing;
			RCGameState->ReplicatedGameModeDelay = -1;
		}
		break;
	}
	case EMatchState::Playing:
	{ 
		

		break; 
	}
	case EMatchState::Ending: 
	{
		UE_LOG(LogTemp, Error, TEXT("%d seconds until RestartServer"), CurGameStateChangeDelay);
		RCGameState->ReplicatedGameModeDelay = CurGameStateChangeDelay;
		--CurGameStateChangeDelay;
		
		if (CurGameStateChangeDelay < 0)
		{
			//플레이어 정리
			for (auto AliveController : AlivePlayerControllers)
			{
				AliveController->ClientRPCReturnToTitle();
			}
			for (auto DeadController : DeadPlayerControllers)
			{
				DeadController->ClientRPCReturnToTitle();
			}
			
			CurGameStateChangeDelay = GAME_CLEAN_DELAY;
			RCGameState->MatchState = EMatchState::Cleaning;
		}
		break; 
	}
	case EMatchState::Cleaning: 
	{
		--CurGameStateChangeDelay;
		if (CurGameStateChangeDelay <= 0)
		{
			MainTimerHandle.Invalidate();
			URCGameInstance* RCGameInstance = GetWorld()->GetGameInstance<URCGameInstance>();
			if (RCGameInstance)
			{
				RCGameInstance->ManageSession(false);
			}

			GetWorld()->ServerTravel(TEXT("WaitingLevel"), true);
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
		ARCGameStateBase* RCGameState = GetGameState<ARCGameStateBase>();
		if (IsValid(RCGameState) == false)
		{
			return;
		}
		RCGameState->AlivePlayerControllerCount = AlivePlayerControllers.Num();

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

void ARCGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
	if (PlayerController)
	{
		APawn* NewPawn = PlayerController->GetPawn();
		if (NewPawn != nullptr)
		{
			SpawnManager->SpawnCharacter(NewPawn);
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
	AlivePlayerControllers.Remove(Controller);
	DeadPlayerControllers.Add(Controller);

	if (AlivePlayerControllers.Num() <= 1)
	{
		ARCGameStateBase* RCGameState = GetGameState<ARCGameStateBase>();
		if (IsValid(RCGameState))
		{
			RCGameState->MatchState = EMatchState::Ending;
			CurGameStateChangeDelay = GAME_ENDING_DELAY;
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
