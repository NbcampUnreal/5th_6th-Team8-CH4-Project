// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "InputAction.h"

#include "RCPlayerCharacter.generated.h"

class UInputAction;
class ATopDownWeaponBase;
class UHealthComponent;
class UStaminaComponent;
class UQuickSlotComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API ARCPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARCPlayerCharacter();

public:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	void RotatePlayerToMouseCursor();
	void SetInteractTarget(AActor* InteractTarget);
	void ClearInteractTarget(AActor* InteractTarget);
	
# pragma region Components
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UQuickSlotComponent> QuickSlotComponent;

# pragma endregion

# pragma region Input
protected:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> IMC_Default;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> DashAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> InteractFAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FlappingMontage;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot1Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot2Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot3Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot4Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot5Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot6Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot7Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> UseSlot8Action;

	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractTarget = nullptr;
private:
	void HandleMoveInput(const FInputActionValue& InValue);
	void HandleDashInput(const FInputActionValue& InValue);
	void HandleSprintPressedInput(const FInputActionValue& InValue);
	void HandleSprintReleasedInput(const FInputActionValue& InValue);
	void HandleInteractFInput(const FInputActionValue& InValue);
	
	void HandleUseQuickSlotInput(int32 SlotIndex);
	void HandleUseSlot1Input(const FInputActionValue& InValue);
	void HandleUseSlot2Input(const FInputActionValue& InValue);
	void HandleUseSlot3Input(const FInputActionValue& InValue);
	void HandleUseSlot4Input(const FInputActionValue& InValue);
	void HandleUseSlot5Input(const FInputActionValue& InValue);
	void HandleUseSlot6Input(const FInputActionValue& InValue);
	void HandleUseSlot7Input(const FInputActionValue& InValue);
	void HandleUseSlot8Input(const FInputActionValue& InValue);

# pragma endregion

# pragma region Ablilty

private:
	const float DefaultMaxWalkSpeed = 500.0f;
	const float SprintMaxWalkSpeed = 1000.0f;
	const float DashMaxWalkSpeed = 3000.0f;

	const float DashCoolDown = 5.0f;
	const float DashStaminaCost = 25.0f;
	
private:
	FVector CurMoveDirection;

	/// <summary>
	/// Have to be Repliacted
	/// </summary>
	bool bCanDash = true;
# pragma endregion

# pragma region Weapon

protected:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<ATopDownWeaponBase> DefaultWeaponClass;

	UPROPERTY()
	ATopDownWeaponBase* CurrentWeapon;

private:
	void HandleFireStarted(const FInputActionValue& InValue);
	void HandleFireStopped(const FInputActionValue& InValue);
# pragma endregion
};

