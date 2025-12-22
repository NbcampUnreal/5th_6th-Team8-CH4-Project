#include "Controller/TitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}	

	if (IsValid(UIWidgetClass))
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (IsValid(UIWidgetInstance))
		{
			UIWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}

	if (TitleBGM)
	{
		UGameplayStatics::SpawnSound2D(this, TitleBGM);
	}
}

void ATitlePlayerController::JoinServer(const FString& InIPAddress)
{
	FName NextLevelName = FName(*InIPAddress);
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
}