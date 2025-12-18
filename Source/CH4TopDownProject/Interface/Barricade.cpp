// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Barricade.h"
#include <Kismet/GameplayStatics.h>
#include <Character/RCPlayerCharacter.h>

ABarricade::ABarricade()
{
}

void ABarricade::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		this->SetOwner(OtherActor);
	}
}

void ABarricade::TakeDamage_Implementation(float Damage, AActor* DamageCauser)
{
}

void ABarricade::Multicast_OnDeath_Implementation()
{
	if (IsNetMode(NM_DedicatedServer))
	{
		return; // 데디 서버에서는 실행 안 함
	}

	if (DeathParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DeathParticle,
			GetActorTransform()
		);
	}
}
