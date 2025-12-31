// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemData/WorldItemBase.h"

#include "Character/RCPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"

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
	Collision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AWorldItemBase::OnSphereOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(
		this,
		&AWorldItemBase::OnSphereEnd);

	InteractionCheckCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCheckCollision->SetSphereRadius(CollisionRadius);
	InteractionCheckCollision->SetupAttachment(Root);
	InteractionCheckCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AWorldItemBase::OnCheckOverlap);
	InteractionCheckCollision->OnComponentEndOverlap.AddDynamic(
		this,
		&AWorldItemBase::OnCheckEndOverlap);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(Root);
	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidget->SetDrawAtDesiredSize(true);
	InteractWidget->SetVisibility(false);

	InteractCheckWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractCheckWidget"));
	InteractCheckWidget->SetupAttachment(StaticMesh);
	InteractCheckWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractCheckWidget->SetDrawAtDesiredSize(true);
	InteractCheckWidget->SetVisibility(false);
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
	if (HasAuthority())
	{
		Multicast_OnDeathEmitter();
		Destroy();
	}
}

void AWorldItemBase::Multicast_OnDeathEmitter_Implementation()
{
	UParticleSystemComponent* Particle = nullptr;

	if (DestroyFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyFX,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.0f)
		);
	}
}

void AWorldItemBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComp,
                                     AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp,
                                     int32 OtherBodyIndex,
                                     bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(OtherActor);
		Player->SetInteractTarget(this);

		/*GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString::Printf(
				TEXT("[WorldItemBase] Begin Overlap : %s"),
				*OtherActor->GetName()
			)
		);*/
	}
}

void AWorldItemBase::OnSphereEnd(UPrimitiveComponent* OverlappedComp,
                                 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(OtherActor);
				Player->ClearInteractTarget(this);
		/*GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString::Printf(
				TEXT("[WorldItemBase] End Overlap : %s"),
				*OtherActor->GetName()
			)
		);*/
	}
}

void AWorldItemBase::OnCheckOverlap(UPrimitiveComponent* OverlappedComp,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex,
                                    bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	if (bCanTakeDamage)
	{
		return;
	}

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ShowInteractCheckWidget(true);
	}
}

void AWorldItemBase::OnCheckEndOverlap(UPrimitiveComponent* OverlappedComp,
                                       AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp,
                                       int32 OtherBodyIndex)
{
	if (bCanTakeDamage)
	{
		return;
	}

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		ShowInteractCheckWidget(false);
	}
}

void AWorldItemBase::SetOutLineEnable(bool Enable)
{
	if (!bCanInteract) return;
	if (bCanTakeDamage) return;
	
	StaticMesh->SetRenderCustomDepth(Enable);
	ShowInteractWidget(Enable);
}

void AWorldItemBase::SetPreviewWidgetEnable(bool Enable)
{
	if (bCanTakeDamage)
	{
		return;
	}

	ShowInteractCheckWidget(Enable);
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

void AWorldItemBase::ShowInteractWidget(bool bVisible)
{
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(bVisible);
	}
}

void AWorldItemBase::ShowInteractCheckWidget(bool bVisible)
{
	if (InteractCheckWidget)
	{
		InteractCheckWidget->SetVisibility(bVisible);
	}
}
