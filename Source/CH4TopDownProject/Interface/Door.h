// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/WorldItemBase.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

UCLASS()
class CH4TOPDOWNPROJECT_API ADoor : public AWorldItemBase
{
	GENERATED_BODY()
	
public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	

protected:
	UFUNCTION()
	virtual void DoorOpenTimeLineFunc(float Output);

	//문열림 여부
	UPROPERTY(ReplicatedUsing = OnRep_DoorState, EditAnywhere, BlueprintReadWrite)
	bool bDoorOpen = false;

private:
	//문열림 연출 실행
	FOnTimelineFloat UpdateFunctionFloat;

	//문열림 연출용 CurveFloat
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UCurveFloat* DoorTimelineCurveFloat;

	//문열림 연출용 Timeline
	UPROPERTY(EditDefaultsOnly, Category = "Door Action", meta = (AllowPrivateAccess = true))
	UTimelineComponent* DoorTimeline;
	
	UFUNCTION(Server, Reliable)
	void Server_ToggleDoor();

	UFUNCTION()
	void OnRep_DoorState();
};

