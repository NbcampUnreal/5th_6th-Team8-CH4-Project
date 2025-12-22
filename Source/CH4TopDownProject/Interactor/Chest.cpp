// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactor/Chest.h"

AChest::AChest()
{
	ItemList.Add(TEXT("Test"), 1);
}
	

void AChest::Interact_Implementation(AActor* Interactor)
{
	// ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(Interactor);
	// if (!Player)
	// 	return;
	//
	// 이런식으로 호출하면 됨
	// Player->OpenChestUI(this);

	
}
