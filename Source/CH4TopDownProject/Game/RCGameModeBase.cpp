
#include "RCGameModeBase.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Game/RCGameInstance.h"

#include "Controller/RCPlayerController.h"

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitGame();
	InitPool();
}

void ARCGameModeBase::InitGame()
{
	URCGameInstance* GI = GetWorld()->GetGameInstance<URCGameInstance>();
	if (GI)
	{
		GI->ManageSession(true);
	}
}

void ARCGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ARCPlayerController* NewPlayerController = Cast<ARCPlayerController>(NewPlayer);
	if (NewPlayerController != nullptr)
	{
		AlivePlayerControllers.Add(NewPlayerController);

		if (AlivePlayerControllers.Num() >= MaxPlayer) {
			UE_LOG(LogTemp, Error, TEXT("Player is full. This Session will be closed..."));

			URCGameInstance* GI = GetWorld()->GetGameInstance<URCGameInstance>();
			if (GI)
			{
				GI->ManageSession(false);
			}
		}
	}
}

void ARCGameModeBase::OnPlayerDeath(APlayerController* Controller)
{
	// remove controller from AlivePlayerControllers

	// left one player 
	// init game
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
