// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LOSVisibilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULOSVisibilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CH4TOPDOWNPROJECT_API ILOSVisibilityInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="LOS")
	void OnLOSBeginVisible();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="LOS")
	void OnLOSEndVisible();
};
