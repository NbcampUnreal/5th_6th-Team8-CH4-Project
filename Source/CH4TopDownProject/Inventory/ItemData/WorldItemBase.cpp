// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemData/WorldItemBase.h"

#include "Character/RCPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWorldItemBase::AWorldItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(Root);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(Root);
	InteractWidget->SetWidgetSpace(EWidgetSpace::World);
	InteractWidget->SetDrawAtDesiredSize(true);
	InteractWidget->SetVisibility(false);
	
}


void AWorldItemBase::BeginPlay()
{
	Super::BeginPlay();
}


void AWorldItemBase::HandleDestroyed()
{
	bCanInteract = false;
	bCanTakeDamage = false;

	OnItemDestroyed();
}

void AWorldItemBase::OnItemDestroyed_Implementation()
{
	Destroy();
}

void AWorldItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(OtherActor);
		Player->SetInteractTarget(this);
	}
}

void AWorldItemBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(OtherActor);
		Player->ClearInteractTarget(this);
	}
}

float AWorldItemBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{

	if (!bCanTakeDamage)
		return 0.f;

	IInteractable::Execute_TakeDamage(this,DamageAmount, DamageCauser);
	
	return DamageAmount;
}


void AWorldItemBase::Interact_Implementation(AActor* Interactor)
{
	if (!bCanInteract)return;

	//Todo 파생 클래스에서 구현
}

void AWorldItemBase::TakeDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (!bCanTakeDamage)return;

	//Todo 파생 클래스에서 구현
}

void AWorldItemBase::ShowInteractWidget(AActor* OtherActor)
{
	
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn || !Pawn->IsLocallyControlled())
		return;

	if (InteractWidget)
	{
		InteractWidget->SetVisibility(true);
	}
}

void AWorldItemBase::HideInteractWidget(AActor* OtherActor)
{
	APawn* Pawn = Cast<APawn>(OtherActor);

	if (!Pawn || !Pawn->IsLocallyControlled())
		return;

	if (InteractWidget)
	{
		InteractWidget->SetVisibility(false);
	}
}
