// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RCGameInstance.h"

#include "OnlineKeyValuePair.h"
#include "OnlineSessionSettings.h"

URCGameInstance::URCGameInstance()
{
}

void URCGameInstance::Init()
{
	UWorld* World = GetWorld();

	if (World)
	{
		if (World->GetNetMode() == NM_DedicatedServer)
		{
			UE_LOG(LogTemp, Error, TEXT("Init - DedicatedServer"));

			GetOnlineSubsystem();
			CreateGameSession();
		}
	}
}

void URCGameInstance::GetOnlineSubsystem()
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

void URCGameInstance::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid()) 
	{
		return;
	}

	OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &URCGameInstance::OnCreateSessionComplete);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	//SessionSettings->bUsesPresence = false;
	//SessionSettings->bAllowJoinViaPresence = false;
	//SessionSettings->bShouldAdvertise = true;
	//SessionSettings->bIsDedicated = true;
	//SessionSettings->bIsLANMatch = false;

	SessionSettings->bIsLANMatch = true;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bIsDedicated = true;

	SessionSettings->Set(FName(TEXT("SessionName")), FString(TEXT("DedicatedServer Session")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName(TEXT("MatchType")), FString(TEXT("Deathmatch")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName(TEXT("SessionStart")), false, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	OnlineSessionInterface->CreateSession(0, FName(TEXT("DedicatedServer Session")), *SessionSettings);
}

void URCGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("Complete Create Seesion : % s"), *SessionName.ToString());
	FNamedOnlineSession* Session = OnlineSessionInterface->GetNamedSession(SessionName);
	if (Session)
	{

	}
}

void URCGameInstance::ManageSession(const bool bIsSessionStarted)
{
	FNamedOnlineSession* Session = OnlineSessionInterface->GetNamedSession(FName(TEXT("DedicatedServer Session")));
	
	if (Session)
	{
		FOnlineSessionSettings& Settings = Session->SessionSettings;
		Settings.Set(FName(TEXT("SessionStart")), bIsSessionStarted, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		OnlineSessionInterface->UpdateSession(Session->SessionName, Settings);

		bool NewIsSessionStarted = bIsSessionStarted;
		Settings.Get(FName("SessionStart"), NewIsSessionStarted);
		UE_LOG(LogTemp, Error, TEXT("%s Session is %s"), *Session->SessionName.ToString(), NewIsSessionStarted ? TEXT("Closed...") : TEXT("Open..."));
	}
}
