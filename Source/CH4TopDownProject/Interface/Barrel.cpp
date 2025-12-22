// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/Barrel.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ABarrel::ABarrel()
{
	ExplosionRadius = 300.0f;
	ExplosionDamage = 50;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetSphereRadius(ExplosionRadius);
	ExplosionCollision->SetupAttachment(StaticMesh);
}

void ABarrel::TakeDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (!bCanTakeDamage)
	{
		return;
	}

	CurrentHP -= Damage;

	if (CurrentHP <= 0)
	{
		if (HasAuthority())
		{
			Explode();
			HandleDestroyed();
		}
		else
		{
			Multicast_OnDeath();
		}
	}
}

void ABarrel::Explode()
{
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == this)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(
			Actor,
			ExplosionDamage,
			nullptr,
			this,
			UDamageType::StaticClass()
		);
	}
}

void ABarrel::Multicast_OnDeath_Implementation()
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