// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Barrel.h"
#include "Net/UnrealNetwork.h"
#include "Barrel.h"
#include "Components/SphereComponent.h"

ABarrel::ABarrel()
{
	ExplosionRadius = 300.0f;
	ExplosionDamage = 300;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetSphereRadius(ExplosionRadius);
	ExplosionCollision->SetupAttachment(StaticMesh);
}

void ABarrel::Interact_Implementation(AActor* Interactor)
{
	if (!bCanInteract)
		return;
}

void ABarrel::TakeDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (!bCanTakeDamage)
		return;
}
