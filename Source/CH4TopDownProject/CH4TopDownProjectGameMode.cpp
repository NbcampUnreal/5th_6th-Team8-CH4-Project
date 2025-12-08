// Copyright Epic Games, Inc. All Rights Reserved.

#include "CH4TopDownProjectGameMode.h"
#include "CH4TopDownProjectPlayerController.h"
#include "CH4TopDownProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACH4TopDownProjectGameMode::ACH4TopDownProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ACH4TopDownProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}