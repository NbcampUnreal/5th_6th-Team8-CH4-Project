
#include "RCPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

//#include "Game/RCGameModeBase.h"

void ARCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	bShowMouseCursor = true;

	FInputModeGameOnly Mode;
	Mode.SetConsumeCaptureMouseDown(false);
	SetInputMode(Mode);
}

//void ARCPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ThisClass, NotificationText);
//}

void ARCPlayerController::OnCharacterDead()
{
	//ARCGameModeBase* GameMode = Cast<ARCGameModeBase>(UGameplayStatics::GetGameMode(this));
	//if (HasAuthority() == true && IsValid(GameMode) == true)
	//{
	//	GameMode->OnCharacterDead(this);
	//}
}
