// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LOSVisibilityComponent.h"

#include "HealthComponent.h"
#include "Character/RCPlayerCharacter.h"
#include "Weapons/WeaponBase.h"
#include "Armor/ArmorBase.h"

ULOSVisibilityComponent::ULOSVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULOSVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
		return;

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

		if (UHealthComponent* HealthComp = OwnerActor->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->OnDeath.AddDynamic(
				this,
				&ULOSVisibilityComponent::HandleOwnerDeath
			);
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
			AWeaponBase* weapon  =Player->GetCurrentWeapon();
			if (weapon)
			{
				weapon->SetActorHiddenInGame(!bVisible);
			}

			AArmorBase* armor = Player->GetCurrentArmor();
			if (armor)
			{
				armor->SetActorHiddenInGame(!bVisible);
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

void ULOSVisibilityComponent::HandleOwnerDeath()
{
	SetOwnerVisible(false);
}



