
#include "RCPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainHUDWidget.h"
#include "Component/HealthComponent.h"
#include "UI/GameResultLayout.h"
#include "GameFramework/GameStateBase.h"
#include <Game/RCGameModeBase.h>

//#include "Game/RCGameModeBase.h"

void ARCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeGameOnly Mode;
	Mode.SetConsumeCaptureMouseDown(false);
	SetInputMode(Mode);
	bShowMouseCursor = true;

	if (IsValid(MainHUDWidgetClass))
	{
		MainHUDWidgetInstance = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
		if (IsValid(MainHUDWidgetInstance))
		{
			MainHUDWidgetInstance->AddToViewport(1);
		}
	}

	APawn* MyPawn = GetPawn();
	if (IsValid(MyPawn))
	{
		UHealthComponent* HealthComp = MyPawn->FindComponentByClass<UHealthComponent>();
		if (IsValid(HealthComp))
		{
			HealthComp->OnDeath.AddDynamic(this, &ARCPlayerController::HandleDeath);
		}
	}

	if (MainHUDWidgetInstance)
	{
		//MainHUDWidgetInstance->ShowNotice(TEXT("Goal: Survive to the end!"));
				
		GetWorld()->GetTimerManager().SetTimer(
			HUDTimerHandle, 
			this, 
			&ARCPlayerController::UpdateHUDTime, 
			1.0f, 
			true
		);
	}
}

void ARCPlayerController::Client_HandleDeath_Implementation()
{
	HandleDeath();
}

void ARCPlayerController::Client_HandleVictory_Implementation()
{
	HandleVictory();
}

void ARCPlayerController::HandleDeath()
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

	if (LoserSound)
	{
		UGameplayStatics::PlaySound2D(this, LoserSound);
	}

	ShowGameResultLayout(LoserLayoutClass);	
}

void ARCPlayerController::HandleVictory()
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

	if (WinnerSound)
	{
		UGameplayStatics::PlaySound2D(this, WinnerSound);
	}

	ShowGameResultLayout(WinnerLayoutClass);	
}

void ARCPlayerController::Client_PlayBlueZoneHitEffect_Implementation()
{
	if (!MainHUDWidgetInstance)
	{
		return;
	}

	MainHUDWidgetInstance->PlayBloodEffect();
}

void ARCPlayerController::ShowGameResultLayout(TSubclassOf<UGameResultLayout> TargetGameResultLayout)
{
	if (IsValid(TargetGameResultLayout))
	{
		UGameResultLayout* GameResultLayout = CreateWidget<UGameResultLayout>(this, TargetGameResultLayout);
		if (GameResultLayout)
		{
			GameResultLayout->AddToViewport(10);

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(GameResultLayout->TakeWidget());
			SetInputMode(InputMode);
			bShowMouseCursor = true;

			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}

}

//void ARCPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ThisClass, NotificationText);
//}


void ARCPlayerController::OnCharacterDead()
{
	ARCGameModeBase* GameMode = Cast<ARCGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (HasAuthority() == true && IsValid(GameMode) == true)
	{
		GameMode->OnPlayerDeath(this);
	}
}

void ARCPlayerController::ClientRPCReturnToTitle_Implementation()
{
	if (IsLocalController() == true)
	{ 
		ClientTravel(TEXT("TitleLevel"),ETravelType::TRAVEL_Absolute);
	}
}


void ARCPlayerController::UpdateHUDTime()
{	
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		float ServerTime = GS->GetServerWorldTimeSeconds();
		MainHUDWidgetInstance->UpdateGameTime(ServerTime);
	}
}