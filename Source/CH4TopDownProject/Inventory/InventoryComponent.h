// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

struct FItemData;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Bag,
	Consumable,
	Equipment_Head,
	Equipment_Body,
	Ammo,
};

UCLASS(BlueprintType)
class UItemContainer : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FItemData> Items;

	UPROPERTY(EditAnywhere)
	int32 MaxSpace = 10;

	bool AddItem(FItemData NewItem);
	bool RemoveItem(int32 Index);
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> DroppedActorClass;

	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;

	UPROPERTY(EditAnywhere)
	int32 Space = 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
