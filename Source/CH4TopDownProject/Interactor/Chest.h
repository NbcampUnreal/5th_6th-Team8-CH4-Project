// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData/WorldItemBase.h"
#include "Chest.generated.h"

USTRUCT(BlueprintType)
struct FChestItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemNum;
};

/**
 * 
 */
UCLASS()
class CH4TOPDOWNPROJECT_API AChest : public AWorldItemBase
{
	GENERATED_BODY()

public:
	AChest();

	UPROPERTY(EditAnywhere, Replicated)
	TArray<FChestItemEntry> ItemListArray;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnSphereEnd(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex)override;

	void SetItem(const TMap<FName, int32>& ItemMap);

	UFUNCTION(Client, Reliable)
	void Client_OpenChestUI(AActor* Interactor);
};
