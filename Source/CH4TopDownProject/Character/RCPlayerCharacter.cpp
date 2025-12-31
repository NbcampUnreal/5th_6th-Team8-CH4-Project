// Fill out your copyright notice in the Description page of Project Settings.

#include "RCPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Weapons/WeaponBase.h"
#include "Weapons/RangeWeapon.h" 
#include "Armor/ArmorBase.h"
#include "Net/UnrealNetwork.h"

#include "Component/HealthComponent.h"
#include "Component/StaminaComponent.h"
#include "Component/QuickSlotComponent.h"
#include "Interface/Interactable.h"
#include "Inventory/ItemData/WorldItemBase.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Inventory/InventoryComponent.h"
#include "UI/OverheadHealthWidget.h"
#include "UI/DamageTextActor.h"
#include "UI/MainHUDWidget.h"

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
	//QuickSlotComponent = CreateDefaultSubobject<UQuickSlotComponent>(TEXT("QuickSlotComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	MinimapSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MinimapSpringArm"));
	MinimapSpringArm->SetupAttachment(RootComponent);
	MinimapSpringArm->SetUsingAbsoluteRotation(true);

	MinimapSpringArm->TargetArmLength = 2000.0f;
	MinimapSpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	MinimapSpringArm->bDoCollisionTest = false;

	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	MinimapCapture->SetupAttachment(MinimapSpringArm, USpringArmComponent::SocketName);
	MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapCapture->OrthoWidth = 10000.0f;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void ARCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	QuickSlotComponent = GetComponentByClass<UInventoryComponent>()->GetQuickSlotComponent();

	if (HasAuthority())
	{
		OnTakePointDamage.AddDynamic(this, &ARCPlayerCharacter::HandlePointDamage);
	}

	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

		UE_LOG(LogTemp, Warning, TEXT("AddMappingContext OK"));
		EILPS->AddMappingContext(IMC_Default, 0);
	}

	if (HasAuthority() && DefaultWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		
		SetCurrentWeapon(GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClass, Params));
		
		if (CurrentWeapon && GetMesh())
		{
			CurrentWeapon->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("WeaponSocket")
			);
			
		}

		CurrentArmor = GetWorld()->SpawnActor<AArmorBase>(
			DefaultArmorClass,
			Params
		);
	}

	if (!IsLocallyControlled())
	{
		if (MinimapCapture)
		{
			MinimapCapture->Deactivate();
			MinimapCapture->SetComponentTickEnabled(false);
		}
	}

	if (OverheadWidget)
	{
		UOverheadHealthWidget* HPWidget = Cast<UOverheadHealthWidget>(OverheadWidget->GetUserWidgetObject());
		if (HPWidget && HealthComponent)
		{
			HPWidget->UpdateHealthBar(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());			
			HealthComponent->OnHealthChanged.AddDynamic(HPWidget, &UOverheadHealthWidget::UpdateHealthBar);
		}
	}

	if (HealthComponent)
	{
		if (HasAuthority())
		{
			HealthComponent->OnDamageReceived.AddDynamic(this, &ARCPlayerCharacter::Multicast_ShowDamageText);
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

	if (UseSlot1Action)
	{
		EIC->BindAction(UseSlot1Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot1Input);
	}
	if (UseSlot2Action)
	{
		EIC->BindAction(UseSlot2Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot2Input);
	}
	if (UseSlot3Action)
	{
		EIC->BindAction(UseSlot3Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot3Input);
	}
	if (UseSlot4Action)
	{
		EIC->BindAction(UseSlot4Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot4Input);
	}
	if (UseSlot5Action)
	{
		EIC->BindAction(UseSlot5Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot5Input);
	}
	if (UseSlot6Action)
	{
		EIC->BindAction(UseSlot6Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot6Input);
	}
	if (UseSlot7Action)
	{
		EIC->BindAction(UseSlot7Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot7Input);
	}
	if (UseSlot8Action)
	{
		EIC->BindAction(UseSlot8Action, ETriggerEvent::Started, this, &ARCPlayerCharacter::HandleUseSlot8Input);
	}

	EIC->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleReloadInput);
}

void ARCPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	const FVector2D InMovementVector = InValue.Get<FVector2D>();
	CurMoveDirection = FVector(InMovementVector.X, InMovementVector.Y, 0);

	AddMovementInput(CurMoveDirection.GetSafeNormal(), 1);
}

void ARCPlayerCharacter::HandleDashInput(const FInputActionValue& InValue)
{
	if (bCanDash == false)
	{
		return;
	}

	if (StaminaComponent && StaminaComponent->GetCurrentStamina() < DashStaminaCost)
	{
		return;
	}

	if (IsValid(FlappingMontage))
	{
		PlayAnimMontage(FlappingMontage, 2.0f);
	}

	Server_HandleDash(CurMoveDirection);

	bCanDash = false;
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]()
	{
		bCanDash = true;
	}), DashCoolDown, false);
}

void ARCPlayerCharacter::HandleSprintPressedInput(const FInputActionValue& InValue)
{
	GetCharacterMovement()->MaxWalkSpeed = SprintMaxWalkSpeed;

	Server_SetSprint(true);
}

void ARCPlayerCharacter::HandleSprintReleasedInput(const FInputActionValue& InValue)
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

	Server_SetSprint(false);
}

void ARCPlayerCharacter::HandleInteractFInput(const FInputActionValue& InValue)
{
	if (!IsValid(CurrentInteractTarget))
		return;

	Server_Interact(CurrentInteractTarget);
}

void ARCPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatePlayerToMouseCursor();

	UpdateAim();
	
	if (IsLocallyControlled())
	{
		const float Speed2D = GetVelocity().Size2D();
		const bool bIsMoving = Speed2D > 10.f;
		const bool bIsFalling = GetCharacterMovement() ? GetCharacterMovement()->IsFalling() : false;

		if (bIsMoving && !bIsFalling)
		{
			StartFootstepLoop();
		}
		else
		{
			StopFootstepLoop();
		}
	}
}

void ARCPlayerCharacter::RotatePlayerToMouseCursor()
{
	if (!IsLocallyControlled()) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FVector WorldOrigin;
	FVector WorldDir;

	if (!PC->DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
		return;

	const float PlaneZ = GetActorLocation().Z;
	const float T = (PlaneZ - WorldOrigin.Z) / WorldDir.Z;

	if (T <= 0.f)
		return;

	const FVector TargetPoint = WorldOrigin + WorldDir * T;
	const FVector Dir = TargetPoint - GetActorLocation();

	const float NewYaw = Dir.Rotation().Yaw;

	SetActorRotation(FRotator(0.f, NewYaw, 0.f));
	Server_SetAimYaw(NewYaw);
}

void ARCPlayerCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARCPlayerCharacter, AimYaw);
	DOREPLIFETIME(ARCPlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(ARCPlayerCharacter, CurrentArmor);
}

void ARCPlayerCharacter::StopSprint()
{
	if (!HasAuthority())
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	if (StaminaComponent)
	{
		StaminaComponent->StopStaminaDrain();
	}

	Client_StopSprint();
}

void ARCPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->Destroy();
		}

		if (CurrentArmor)
		{
			CurrentArmor->Destroy();
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void ARCPlayerCharacter::Server_Interact_Implementation(AWorldItemBase* Target)
{
	if (!IsValid(Target)) return;

	if (Target->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(Target, this);
	}
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

void ARCPlayerCharacter::Server_SetSprint_Implementation(bool bIsSprinting)
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintMaxWalkSpeed;

		if (StaminaComponent)
		{
			StaminaComponent->StartStaminaDrain(10.0f);
		}
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

		if (StaminaComponent)
		{
			StaminaComponent->StopStaminaDrain();
		}
	}
}

void ARCPlayerCharacter::SetInteractTarget(AWorldItemBase* InteractTarget)
{
	if (!IsLocallyControlled())
		return;

	if (IsValid(CurrentInteractTarget))
	{
		CurrentInteractTarget->SetOutLineEnable(false);
	}

	CurrentInteractTarget = InteractTarget;
	CurrentInteractTarget->SetOutLineEnable(true);
}

void ARCPlayerCharacter::ClearInteractTarget(AWorldItemBase* InteractTarget)
{
	if (!IsLocallyControlled())
		return;

	if (CurrentInteractTarget == InteractTarget)
	{
		CurrentInteractTarget->SetOutLineEnable(false);
		CurrentInteractTarget = nullptr;
	}
}

void ARCPlayerCharacter::Server_HandleDash_Implementation(FVector DashDirection)
{
	if (!StaminaComponent || !StaminaComponent->ConsumeStamina(DashStaminaCost))
	{
		return;
	}

	if (IsValid(FlappingMontage))
	{
		PlayAnimMontage(FlappingMontage, 2.0f);
	}

	LaunchCharacter(DashDirection * DashMaxWalkSpeed, true, false);
	Multicast_PlayDashSFX(GetActorLocation());
}

void ARCPlayerCharacter::Client_StopSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
}

void ARCPlayerCharacter::HandleFireStarted(const FInputActionValue& InValue)
{
	bIsFirstButtonDown = true;

	if (!CurrentWeapon) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit)) return;

	FVector Target = Hit.ImpactPoint;
	Target.Z += 80.f;

	CurrentWeapon->StartAttack(Target);
}

void ARCPlayerCharacter::HandleFireStopped(const FInputActionValue& InValue)
{
	bIsFirstButtonDown = false;

	if (CurrentWeapon)
	{
		CurrentWeapon->StopAttack();
	}
}

void ARCPlayerCharacter::HandleUseQuickSlotInput(int32 SlotIndex)
{
	if (QuickSlotComponent)
	{
		QuickSlotComponent->UseQuickSlot(SlotIndex);
	}
}

void ARCPlayerCharacter::HandleUseSlot1Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(0);
}

void ARCPlayerCharacter::HandleUseSlot2Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(1);
}

void ARCPlayerCharacter::HandleUseSlot3Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(2);
}

void ARCPlayerCharacter::HandleUseSlot4Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(3);
}

void ARCPlayerCharacter::HandleUseSlot5Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(4);
}

void ARCPlayerCharacter::HandleUseSlot6Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(5);
}

void ARCPlayerCharacter::HandleUseSlot7Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(6);
}

void ARCPlayerCharacter::HandleUseSlot8Input(const FInputActionValue& InValue)
{
	HandleUseQuickSlotInput(7);
}

void ARCPlayerCharacter::HandleReloadInput(const FInputActionValue& InValue)
{
	if (!CurrentWeapon) return;

	CurrentWeapon->StartReload();
}

void ARCPlayerCharacter::SetCurrentWeapon(AActor* weapon)
{
	AWeaponBase* NewWeapon = Cast<AWeaponBase>(weapon);
	if (!NewWeapon) return;
	
	if (CurrentWeapon) {
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	if (!HasAuthority())
	{
		Server_SetCurrentWeapon(NewWeapon);
		return;
	}

	Server_SetCurrentWeapon(NewWeapon);

	
}

void ARCPlayerCharacter::Server_SetCurrentWeapon_Implementation(AWeaponBase* NewWeapon)
{
	if (!NewWeapon || !GetMesh()) return;

	CurrentWeapon = NewWeapon;

	CurrentWeapon->SetOwner(this);
	CurrentWeapon->SetInstigator(this);

	CurrentWeapon->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("WeaponSocket")
	);
	

	CurrentWeapon->ForceNetUpdate();
	ForceNetUpdate();

	CurrentWeapon->SetActorEnableCollision(false);
}

void ARCPlayerCharacter::UpdateAim()
{
	if (!IsLocallyControlled())
		return;

	if (!bIsFirstButtonDown)
		return;

	if (!CurrentWeapon)
		return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		return;

	FVector Target = Hit.ImpactPoint;
	Target.Z += 80.f; 

	CurrentWeapon->Server_UpdateAim(Target);

}

void ARCPlayerCharacter::SetArmor(AActor* Armor)
{
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

		CurrentWeapon->SetActorEnableCollision(false);
		
		CurrentWeapon->SetActorHiddenInGame(!IsLocallyControlled());
	}
	
	UpdateAmmoUI();
}

void ARCPlayerCharacter::OnRep_CurrentArmor()
{
	if (CurrentArmor && GetMesh())
	{
		CurrentArmor->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("ArmorChestSocket")
		);

		CurrentArmor->SetActorEnableCollision(false);
	}
}

void ARCPlayerCharacter::Server_SetAimYaw_Implementation(float NewYaw)
{
	AimYaw = NewYaw;
	SetActorRotation(FRotator(0.f, AimYaw, 0.f));
}
void ARCPlayerCharacter::PlayFootstepOnce()
{
	if (!IsLocallyControlled()) return;
	if (!FootstepSound) return;

	const float Speed2D = GetVelocity().Size2D();
	if (Speed2D < 10.f) return;

	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling()) return;

	UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());

	Server_PlayFootstep(GetActorLocation());
}

void ARCPlayerCharacter::Server_PlayFootstep_Implementation(const FVector& Loc)
{
	Multicast_PlayFootstep(Loc);
}

void ARCPlayerCharacter::Multicast_PlayFootstep_Implementation(const FVector& Loc)
{
	if (!IsLocallyControlled() && FootstepSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, Loc);
	}
}

void ARCPlayerCharacter::StartFootstepLoop()
{
	if (!IsLocallyControlled()) return;
	if (!FootstepSound) return;

	if (!GetWorldTimerManager().IsTimerActive(FootstepTimerHandle))
	{
		PlayFootstepOnce();

		GetWorldTimerManager().SetTimer(
			FootstepTimerHandle,
			this,
			&ARCPlayerCharacter::PlayFootstepOnce,
			FootstepInterval,
			true
		);
	}
}

void ARCPlayerCharacter::StopFootstepLoop()
{
	if (!IsLocallyControlled()) return;
	GetWorldTimerManager().ClearTimer(FootstepTimerHandle);
}

void ARCPlayerCharacter::Multicast_PlayDashSFX_Implementation(const FVector& Loc)
{
	if (GetNetMode() == NM_DedicatedServer) return;
	if (!DashSound) return;

	UGameplayStatics::PlaySoundAtLocation(this, DashSound, Loc);
}

void ARCPlayerCharacter::Multicast_ShowDamageText_Implementation(float Damage, FVector Location)
{
	if (DamageTextClass)
	{
		FVector SpawnLocation = Location + FVector(FMath::RandRange(-20.f, 20.f), FMath::RandRange(-20.f, 20.f), 100.f);

		ADamageTextActor* DamageActor = GetWorld()->SpawnActor<ADamageTextActor>(DamageTextClass, SpawnLocation, FRotator::ZeroRotator);
		if (DamageActor)
		{
			DamageActor->InitializeDamage(Damage);
		}
	}
}

void ARCPlayerCharacter::SetMainHUDWidget(UMainHUDWidget* InHUDWidget)
{
	MainHUDWidgetInstance = InHUDWidget;

	UpdateAmmoUI();
}

void ARCPlayerCharacter::UpdateAmmoUI()
{
	if (!IsLocallyControlled() || !MainHUDWidgetInstance)
	{
		return;
	}

	ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(CurrentWeapon);
	int32 SlotIndex = InventoryComponent->GetCurrentWeaponIndex();

	if (RangeWeapon && (SlotIndex == 0 || SlotIndex == 1))
	{
		int32 CurrentAmmo = RangeWeapon->GetCurrentAmmo();

		int32 TotalAmmoInInventory = 0;
		if (InventoryComponent)
		{
			FName TargetAmmoID = RangeWeapon->GetAmmoItemID();
			TotalAmmoInInventory = InventoryComponent->GetTotalItemCountByID(TargetAmmoID);
		}

		MainHUDWidgetInstance->UpdateSlotAmmo(SlotIndex, CurrentAmmo, TotalAmmoInInventory);
	}
	else
	{		
		MainHUDWidgetInstance->HideAllAmmoUI();
	}
}