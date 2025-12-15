// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/LOSVisibilityInterface.h"
#include "LOSVisibilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API ULOSVisibilityComponent : public UActorComponent, public ILOSVisibilityInterface
{
	GENERATED_BODY()

public:	
	ULOSVisibilityComponent();

protected:
	virtual void BeginPlay() override;

private:
	void SetOwnerVisible(bool bVisible) const;

public:
	virtual void OnLOSBeginVisible_Implementation() override;
	virtual void OnLOSEndVisible_Implementation() override;

		
};
