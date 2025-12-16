// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RCPlayerController.generated.h"

class UMainHUDWidget;

UCLASS()
class CH4TOPDOWNPROJECT_API ARCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//public:
//	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
//	FText NotificationText;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	TSubclassOf<UUserWidget> NotificationTextUIClass;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainHUDWidget> MainHUDWidgetInstance;

public:
	void OnCharacterDead();
};
