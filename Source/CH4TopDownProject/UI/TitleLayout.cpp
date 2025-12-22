#include "UI/TitleLayout.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/TitlePlayerController.h"

UTitleLayout::UTitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTitleLayout::NativeConstruct()
{
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UTitleLayout::OnPlayButtonClicked()
{
	//ATitlePlayerController* PlayerController = GetOwningPlayer<ATitlePlayerController>();
	//if (IsValid(PlayerController))
	//{
	//	FText ServerIP = ServerIPEditableText->GetText();
	//	PlayerController->JoinServer(ServerIP.ToString());
	//}

	ATitlePlayerController* OwningPlayerController = GetOwningPlayer<ATitlePlayerController>();
	if (IsValid(OwningPlayerController))
	{
		OwningPlayerController->OnClickFindSession();
	}
}

void UTitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}