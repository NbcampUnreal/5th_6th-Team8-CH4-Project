// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemData/WorldItemBase.h"

#include "Character/RCPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
AWorldItemBase::AWorldItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CurrentHP = MaxHP;

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
		
		GEngine->AddOnScreenDebugMessage(
			 -1,
			 2.f,
			 FColor::Green,
			 FString::Printf(
				 TEXT("[WorldItemBase] Begin Overlap : %s"),
				 *OtherActor->GetName()
			 )
		 );
	}
}

void AWorldItemBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(OtherActor);
		Player->ClearInteractTarget(this);
		
		   GEngine->AddOnScreenDebugMessage(
                -1,
                2.f,
                FColor::Green,
                FString::Printf(
                    TEXT("[WorldItemBase] End Overlap : %s"),
                    *OtherActor->GetName()
                )
            );
	}
}

void AWorldItemBase::SetOutLineEnable(bool Enable)
{
	if (!bCanInteract) return;
	
	StaticMesh->SetRenderCustomDepth(Enable);
	ShowInteractWidget(Enable);
}

float AWorldItemBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                 class AController* EventInstigator, AActor* DamageCauser)
{
	if (!bCanTakeDamage)
		return 0.f;

	IInteractable::Execute_TakeDamage(this, DamageAmount, DamageCauser);
	
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

void AWorldItemBase::ShowInteractWidget( bool bVisible)
{
	
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(bVisible);
	}
}


