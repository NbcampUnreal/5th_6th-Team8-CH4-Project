// Copyright Epic Games, Inc. All Rights Reserved.

#include "CH4TopDownProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Weapon/TopDownWeaponBase.h"

ACH4TopDownProjectCharacter::ACH4TopDownProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACH4TopDownProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;

		CurrentWeapon = GetWorld()->SpawnActor<ATopDownWeaponBase>(DefaultWeaponClass, Params);

		if (CurrentWeapon)
		{


			if (GetMesh())
			{
				CurrentWeapon->AttachToComponent(
					GetMesh(),
					FAttachmentTransformRules::KeepRelativeTransform
					// , TEXT("hand_r_weapon")  // 소켓 만들면 이 줄 활성화
				);

				CurrentWeapon->SetActorRelativeLocation(FVector(50.f, 0.f, 50.f));
			}
			else
			{
				CurrentWeapon->AttachToComponent(
					RootComponent,
					FAttachmentTransformRules::KeepRelativeTransform
				);
				CurrentWeapon->SetActorRelativeLocation(FVector(50.f, 0.f, 50.f));
			}
		}
	}
}

void ACH4TopDownProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ACH4TopDownProjectCharacter::StartWeaponFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ACH4TopDownProjectCharacter::StopWeaponFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}