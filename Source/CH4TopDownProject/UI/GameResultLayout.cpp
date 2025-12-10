#include "UI/GameResultLayout.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/TitlePlayerController.h"

UGameResultLayout::UGameResultLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameResultLayout::NativeConstruct()
{
	TitleButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnTitleButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UGameResultLayout::OnTitleButtonClicked()
{
	if (GetWorld())
	{		
		const FString TitleLevelName = TEXT("TitleLevel");
		UGameplayStatics::OpenLevel(GetWorld(), FName(*TitleLevelName));
	}
}

void UGameResultLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}