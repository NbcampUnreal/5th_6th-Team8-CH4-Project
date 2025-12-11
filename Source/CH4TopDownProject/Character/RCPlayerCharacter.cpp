// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
}

void ARCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleMoveInput);
	//EIC->BindAction(IA_Aim, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleAimInput);

	EIC->BindAction(IA_InteractF, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleInteractFInput);
}

void ARCPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	const FVector2D InMovementVector = InValue.Get<FVector2D>();
	const FVector MoveDirection = FVector(InMovementVector.X, InMovementVector.Y, 0);

	AddMovementInput(MoveDirection.GetSafeNormal(), 1);
}

void ARCPlayerCharacter::HandleInteractFInput(const FInputActionValue& InValue)
{

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
