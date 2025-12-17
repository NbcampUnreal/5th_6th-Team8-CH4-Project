// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Weapon/TopDownWeaponBase.h"
#include "Net/UnrealNetwork.h"

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
	
	if (HasAuthority())
	{
		OnTakePointDamage.AddDynamic(this, &ARCPlayerCharacter::HandlePointDamage);
	}

	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

		UE_LOG(LogTemp, Warning, TEXT("AddMappingContext OK"));
		EILPS->AddMappingContext(IMC_Default, 0);
	}

	if (HasAuthority() && DefaultWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;

		CurrentWeapon = GetWorld()->SpawnActor<ATopDownWeaponBase>(DefaultWeaponClass, Params);

		if (CurrentWeapon && GetMesh())
		{
			CurrentWeapon->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("WeaponSocket")
			);
		}
	}
}

void ARCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleMoveInput);
	EIC->BindAction(DashAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleDashInput);

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

void ARCPlayerCharacter::HandleInteractFInput(const FInputActionValue& InValue)
{
	UE_LOG(LogTemp, Display, TEXT("HandleInteractFInput"));
}

void ARCPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatePlayerToMouseCursor();
}

void ARCPlayerCharacter::RotatePlayerToMouseCursor()
{
	/*
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
	*/

	if (!IsLocallyControlled()) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		const float NewYaw = (Hit.ImpactPoint - GetActorLocation()).Rotation().Yaw;

		SetActorRotation(FRotator(0.f, NewYaw, 0.f));

		Server_SetAimYaw(NewYaw);
	}


}

void ARCPlayerCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARCPlayerCharacter, AimYaw);
	DOREPLIFETIME(ARCPlayerCharacter, CurrentWeapon);
}

void ARCPlayerCharacter::HandlePointDamage(
	AActor* DamagedActor,
	float Damage,
	AController* InstigatedBy,
	FVector HitLocation,
	UPrimitiveComponent* FHitComponent,
	FName BoneName,
	FVector ShotFromDirection,
	const UDamageType* DamageType,
	AActor* DamageCauser
)
{
	UE_LOG(LogTemp, Error,
		TEXT("[Character][Server][TakePointDamage] Victim=%s Damage=%.1f Causer=%s Bone=%s"),
		*GetName(),
		Damage,
		*GetNameSafe(DamageCauser),
		*BoneName.ToString()
	);

}

void ARCPlayerCharacter::HandleFireStarted(const FInputActionValue& InValue)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Fire Started"));
	}
	UE_LOG(LogTemp, Warning, TEXT("HandleFireStarted called"));

	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon is null"));
	}
}

void ARCPlayerCharacter::HandleFireStopped(const FInputActionValue& InValue)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ARCPlayerCharacter::OnRep_CurrentWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_CurrentWeapon: %s"),
		*GetNameSafe(CurrentWeapon));

	if (CurrentWeapon && GetMesh())
	{
		CurrentWeapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("WeaponSocket")
		);
	}
}

void ARCPlayerCharacter::Server_SetAimYaw_Implementation(float NewYaw)
{
	AimYaw = NewYaw;
	SetActorRotation(FRotator(0.f, AimYaw, 0.f));
}