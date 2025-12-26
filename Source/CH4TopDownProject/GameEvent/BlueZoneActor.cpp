#include "GameEvent/BlueZoneActor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"

ABlueZoneActor::ABlueZoneActor()
{
	PrimaryActorTick.bCanEverTick = true;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	bReplicates = true;
	bAlwaysRelevant = true;

	CurrentRadius = BlueZoneRadius;
}

void ABlueZoneActor::BeginPlay()
{
	Super::BeginPlay();

	CurrentRadius = BlueZoneRadius;

	// Will be erased
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			BlueZoneDamageHandle,
			this,
			&ABlueZoneActor::ApplyBlueZoneDamage,
			DamageInterval,
			true
		);

		GetWorld()->GetTimerManager().SetTimer(
			ShrinkTimerHandle,
			this,
			&ABlueZoneActor::StartShrink,
			WaitTime,
			false
		);
	}
}

void ABlueZoneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DrawDebugBlueZone();

	float Scale = CurrentRadius / 50.0f;
	VisualMesh->SetWorldScale3D(FVector(Scale, Scale, Scale));
}

void ABlueZoneActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlueZoneActor, CurrentRadius);
}

void ABlueZoneActor::StartShrink()
{
	if (!HasAuthority())
	{
		return;
	}

	BlueZoneState = EBlueZoneState::Shrinking;
	ShrinkElapsedTime = 0.0f;
		
	GetWorld()->GetTimerManager().SetTimer(
		ShrinkTimerHandle,
		this,
		&ABlueZoneActor::UpdateShrink,
		0.05f,
		true
	);
}

void ABlueZoneActor::UpdateShrink()
{
	if (!HasAuthority())
	{
		return;
	}

	ShrinkElapsedTime += 0.05f;

	float Alpha = FMath::Clamp(ShrinkElapsedTime / ShrinkDuration, 0.0f, 1.0f);
	CurrentRadius = FMath::Lerp(BlueZoneRadius, TargetRadius, Alpha);

	if (Alpha >= 1.0f)
	{
		CurrentRadius = TargetRadius;
		BlueZoneState = EBlueZoneState::Finished;

		GetWorld()->GetTimerManager().ClearTimer(ShrinkTimerHandle);
	}
}

void ABlueZoneActor::ApplyBlueZoneDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	if (BlueZoneState == EBlueZoneState::Waiting)
	{
		return;
	}

	for (TActorIterator<ACharacter> It(GetWorld()); It; ++It)
	{
		ACharacter* TargetCharacter = *It;

		if (!TargetCharacter || !TargetCharacter->IsPlayerControlled())
		{
			continue;
		}

		const float Distance = FVector::Dist2D(GetActorLocation(), TargetCharacter->GetActorLocation());

		if (Distance > CurrentRadius)
		{
			UGameplayStatics::ApplyDamage(
				TargetCharacter,
				BlueZoneDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void ABlueZoneActor::DrawDebugBlueZone()
{
	DrawDebugCircle(
		GetWorld(),
		GetActorLocation(),
		CurrentRadius,
		64,
		FColor::Blue,
		false,
		-1.0f,
		0,
		10.0f,
		FVector(1, 0, 0),
		FVector(0, 1, 0),
		false
	);
}

void ABlueZoneActor::ActivateBlueZone()
{
	if (HasAuthority())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		BlueZoneDamageHandle,
		this,
		&ABlueZoneActor::ApplyBlueZoneDamage,
		DamageInterval,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		ShrinkTimerHandle,
		this,
		&ABlueZoneActor::StartShrink,
		WaitTime,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Activate Blue Zone!"));
}