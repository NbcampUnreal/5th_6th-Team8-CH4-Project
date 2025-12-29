// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactor/Chest.h"

#include "Character/RCPlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

AChest::AChest()
{
	//ItemList.Add(TEXT("Test"), 1);
}


void AChest::Interact_Implementation(AActor* Interactor)
{
	//ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(Interactor);
	//if (!Player)
	//	return;
	//
	//// 이런식으로 호출하면 됨
	//UInventoryComponent* Inventory = Player->GetComponentByClass<UInventoryComponent>();
	//Inventory->OpenChestUI(this);

	if (!HasAuthority()) return;

	SetOwner(Interactor);
	Client_OpenChestUI(Interactor);
}

void AChest::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChest, ItemListArray);
}

void AChest::OnSphereEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex)
{
	Super::OnSphereEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(OtherActor);
	if (!Player)
		return;

	UInventoryComponent* Inventory = Player->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)return;
	Inventory->CloseChestUI();
}

void AChest::SetItem(const TMap<FName, int32>& ItemMap)
{
	if (!HasAuthority()) return;

	ItemListArray.Empty();
	for (const auto& Pair : ItemMap)
	{
		ItemListArray.Add({Pair.Key, Pair.Value});
	}

	int diff = 8 - ItemMap.Num();

	for (int i = 0; i < diff; ++i)
	{
		ItemListArray.Add({"", 0});
	}
}

void AChest::Client_OpenChestUI_Implementation(AActor* Interactor)
{
	ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(Interactor);
	if (!Player)
		return;

	// 이런식으로 호출하면 됨
	UInventoryComponent* Inventory = Player->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)return;
	Inventory->OpenChestUI(this);

	if (ItemListArray.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
		                                 TEXT("EMPTY"));
	}

	for (const FChestItemEntry& Entry : ItemListArray)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
		                                 FString::Printf(TEXT("%s : %d"), *Entry.ItemName.ToString(), Entry.ItemNum));
	}
}
