// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LOSVisibilityComponent.h"

#include "Character/RCPlayerCharacter.h"
#include "Weapon/TopDownWeaponBase.h"


ULOSVisibilityComponent::ULOSVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void ULOSVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->SetActorHiddenInGame(true);

		if (APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
			{
				if (PC->IsLocalController())
				{
					OwnerActor->SetActorHiddenInGame(false);
				}
			}
		}
	}
}

void ULOSVisibilityComponent::SetOwnerVisible(bool bVisible) const
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorHiddenInGame(!bVisible);

		if (ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(Owner))
		{
			ATopDownWeaponBase* weapon  =Player->GetCurrentWeapon();
			if (weapon)
			{
				weapon->SetActorHiddenInGame(!bVisible);
			}
		}
	}
}

void ULOSVisibilityComponent::OnLOSBeginVisible_Implementation()
{
	SetOwnerVisible(true);
}

void ULOSVisibilityComponent::OnLOSEndVisible_Implementation()
{
	SetOwnerVisible(false);
}



