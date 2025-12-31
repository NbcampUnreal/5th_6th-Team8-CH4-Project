#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuickSlotComponent.generated.h"

class UInventoryComponent;
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

	UFUNCTION(BlueprintCallable)
	void UseQuickSlot(int32 SlotIndex);

	UFUNCTION( BlueprintCallable, Category = "QuickSlot")
	void Server_SetQuickSlot(int32 SlotIndex, FName NewItemID, EItemType NewItemType);

	UFUNCTION()
	void Server_UseQuickSlot(int32 SlotIndex);

	void UpdateSlotCount(FName ItemID, int32 NewCount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetQuickSlot(UUserWidget* widget);

private:
	UUserWidget* SelectItemWidget;
	UInventoryComponent* GetInventoryComponent() const;

protected:	
	UPROPERTY()
	TArray<FQuickSlotItemData> QuickSlotData;
	
	UPROPERTY(EditDefaultsOnly, Category = "QuickSlot")
	int32 NumQuickSlots = 8;
};
