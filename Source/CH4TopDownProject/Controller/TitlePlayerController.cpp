#include "Controller/TitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Online/OnlineSessionNames.h" 
#include "OnlineSessionSettings.h"

ATitlePlayerController::ATitlePlayerController()
	: FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ATitlePlayerController::OnFindSessionComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ATitlePlayerController::OnJoinSessionComplate))
{
}

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

	GetOnlineSubsystem();
}

void ATitlePlayerController::GetOnlineSubsystem()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Subsystem Name : % s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}
}

void ATitlePlayerController::OnClickFindSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}

		return;
	}

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	//SessionSearch->QuerySettings.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);

	if (GEngine)
	{
		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Find Sessions...")));
	}

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void ATitlePlayerController::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid() || !bWasSuccessful)
	{
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Found Session Count : %d"), SessionSearch->SearchResults.Num());
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Found Session Count : %d"), SessionSearch->SearchResults.Num()));
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		FString SessionName;
		Result.Session.SessionSettings.Get(FName("SessionName"), SessionName);
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		bool bSessionStart;
		Result.Session.SessionSettings.Get(FName("SessionStart"), bSessionStart);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("%s Session: %s(%s)- %s"), *User, *SessionName, *Id, (bSessionStart ? TEXT("Started") : TEXT("Waitting"))));
		}
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		FString SessionName;
		Result.Session.SessionSettings.Get(FName("SessionName"), SessionName);
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		bool bSessionStart;
		Result.Session.SessionSettings.Get(FName("SessionStart"), bSessionStart);

		if (true && bSessionStart == false)
		{
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (IsValid(LocalPlayer))
			{
				OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), FName(*SessionName), Result);
				return;
			}
		}
	}
}

void ATitlePlayerController::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Joined Session Name : %s(%s)"), *SessionName.ToString(), *Address));
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void ATitlePlayerController::JoinServer(const FString& InIPAddress)
{
	FName NextLevelName = FName(*InIPAddress);
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
}