#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Interfaces/OnlineSessionInterface.h"

#include "TitlePlayerController.generated.h"

class USoundBase;

UCLASS()
class CH4TOPDOWNPROJECT_API ATitlePlayerController : public APlayerController
{
	GENERATED_BODY()
		
public:
	ATitlePlayerController();
	virtual void BeginPlay() override;

protected:
	void GetOnlineSubsystem();

	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

# pragma region FindSession
public:
	UFUNCTION(BlueprintCallable)
	void OnClickFindSession();

private:
	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	void OnFindSessionComplete(bool bWasSuccessful);

#pragma endregion

# pragma region JoinSession

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

#pragma endregion
public:
	void JoinServer(const FString& InIPAddress);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> TitleBGM;
};