// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Weapon/TopDownWeaponBase.h"

#include "Component/HealthComponent.h"
#include "Component/StaminaComponent.h"
#include "Interface/Interactable.h"

ARCPlayerCharacter::ARCPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1500.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
}

void ARCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

		EILPS->AddMappingContext(IMC_Default, 0);
	}

	if (DefaultWeaponClass && GetWorld())
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
					FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					TEXT("WeaponSocket")
				);
			}
		}
	}
}

void ARCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleMoveInput);
	EIC->BindAction(DashAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleDashInput);
	
	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleSprintPressedInput);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARCPlayerCharacter::HandleSprintReleasedInput);

	EIC->BindAction(InteractFAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleInteractFInput);

	EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleFireStarted);
	EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ARCPlayerCharacter::HandleFireStopped);
}

void ARCPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	const FVector2D InMovementVector = InValue.Get<FVector2D>();
	CurMoveDirection = FVector(InMovementVector.X, InMovementVector.Y, 0);

	AddMovementInput(CurMoveDirection.GetSafeNormal(), 1);
}

void ARCPlayerCharacter::HandleDashInput(const FInputActionValue& InValue)
{
	if (IsValid(FlappingMontage) == false || bCanDash == false)
	{
		return;
	}

	if (!StaminaComponent || !StaminaComponent->ConsumeStamina(DashStaminaCost))
	{
		return;
	}

	if (IsValid(GetMesh()) && IsValid(GetMesh()->GetAnimInstance()))
	{
		GetMesh()->GetAnimInstance()->Montage_Play(FlappingMontage, 2.0f);
	}

	LaunchCharacter(CurMoveDirection * DashMaxWalkSpeed, true, false);

	bCanDash = false;
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]()
		{
			bCanDash = true;
		}), DashCoolDown, false);
}

void ARCPlayerCharacter::HandleSprintPressedInput(const FInputActionValue& InValue)
{
	if (StaminaComponent)
	{
		StaminaComponent->StartStaminaDrain(10.0f);
	}

	GetCharacterMovement()->MaxWalkSpeed = SprintMaxWalkSpeed;
}

void ARCPlayerCharacter::HandleSprintReleasedInput(const FInputActionValue& InValue)
{
	if (StaminaComponent)
	{
		StaminaComponent->StopStaminaDrain();
	}

	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
}

void ARCPlayerCharacter::HandleInteractFInput(const FInputActionValue& InValue)
{
	if (!IsValid(CurrentInteractTarget))
		return;

	if (CurrentInteractTarget->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(CurrentInteractTarget, this);
	}
}

void ARCPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatePlayerToMouseCursor();
}

void ARCPlayerCharacter::RotatePlayerToMouseCursor()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		if (HitResult.bBlockingHit) {
			FRotator NewRot = (HitResult.ImpactPoint - GetActorLocation()).Rotation();

			SetActorRotation(FRotator(0, NewRot.Yaw, 0));
		}
	}
}

void ARCPlayerCharacter::SetInteractTarget(AActor* InteractTarget)
{
	CurrentInteractTarget = InteractTarget;
}

void ARCPlayerCharacter::ClearInteractTarget(AActor* InteractTarget)
{
	if (CurrentInteractTarget == InteractTarget)
	{
		CurrentInteractTarget = nullptr;
	}
	
}

void ARCPlayerCharacter::HandleFireStarted(const FInputActionValue& InValue)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ARCPlayerCharacter::HandleFireStopped(const FInputActionValue& InValue)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}