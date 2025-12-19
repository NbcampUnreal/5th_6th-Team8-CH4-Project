#include "Component/StaminaComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/RCPlayerCharacter.h"

UStaminaComponent::UStaminaComponent()
	: CurrentStamina(100.0f)
	, StaminaRecoveryRate(10.0f)
	, CurrentDrainRate(0.0f)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bCanEverTick = true;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		CurrentStamina = MaxStamina;

		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	}
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority())
	{
		return;
	}	

	bool bIsDraining = CurrentDrainRate > 0.0f;
	bool bIsMoving = false;

	if (ARCPlayerCharacter* PlayerCharacter = Cast<ARCPlayerCharacter>(GetOwner()))
	{
		bIsMoving = PlayerCharacter->GetVelocity().Size() > 5.0f;
	}

	float NewStamina = CurrentStamina;

	if (bIsDraining && bIsMoving)
	{
		NewStamina -= CurrentDrainRate * DeltaTime;
	}
	else
	{
		NewStamina += StaminaRecoveryRate * DeltaTime;
	}

	SetStamina(FMath::Clamp(NewStamina, 0.0f, MaxStamina));

	if (CurrentStamina <= 0.0f && bIsDraining)
	{
		if (ARCPlayerCharacter* PlayerCharacter = Cast<ARCPlayerCharacter>(GetOwner()))
		{
			PlayerCharacter->StopSprint();
		}
	}
}

bool UStaminaComponent::ConsumeStamina(float Amount)
{	
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	if (CurrentStamina >= Amount)
	{
		SetStamina(CurrentStamina - Amount);

		return true;
	}

	return false;
}

void UStaminaComponent::StartStaminaDrain(float DrainRate)
{	
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	CurrentDrainRate = FMath::Max(0.0f, DrainRate);
}

void UStaminaComponent::StopStaminaDrain()
{	
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	CurrentDrainRate = 0.0f;
}

float UStaminaComponent::GetCurrentStamina() const
{
	return CurrentStamina;
}

float UStaminaComponent::GetMaxStamina() const
{
	return MaxStamina;
}

void UStaminaComponent::SetStamina(float NewStamina)
{	
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	float OldStamina = CurrentStamina;
	CurrentStamina = NewStamina;

	OnRep_CurrentStamina(OldStamina);
}

void UStaminaComponent::OnRep_CurrentStamina(float OldStamina)
{
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}