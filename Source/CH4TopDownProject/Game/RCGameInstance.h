// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"

#include "RCGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API URCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URCGameInstance();

public:
	virtual void Init() override;

protected:
	void GetOnlineSubsystem();
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;

# pragma region CreateSession
public:
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);


private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
#pragma endregion

# pragma region Open/Cloase Session

public:
	void ManageSession(bool bIsSessionStarted);

# pragma endregion
	
};
