#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuickSlotComponent.generated.h"

struct FQuickSlotItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickSlotDataChangedSignature, const TArray<FQuickSlotItemData>&, NewSlotData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UQuickSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	UQuickSlotComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_QuickSlotData();

public:
	UPROPERTY(BlueprintAssignable)
	FOnQuickSlotDataChangedSignature OnQuickSlotDataChanged;

	UFUNCTION(BlueprintPure, Category = "QuickSlot")
	const TArray<FQuickSlotItemData>& GetQuickSlotData() const;

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void SetSlotItem(int32 SlotIndex, FName NewItemID, EItemType NewItemType, int32 NewStackCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UseQuickSlot(int32 SlotIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	

protected:	
	UPROPERTY(ReplicatedUsing = OnRep_QuickSlotData, VisibleAnywhere, Category = "QuickSlot")
	TArray<FQuickSlotItemData> QuickSlotData;
	
	UPROPERTY(EditDefaultsOnly, Category = "QuickSlot")
	int32 NumQuickSlots = 8;
};
