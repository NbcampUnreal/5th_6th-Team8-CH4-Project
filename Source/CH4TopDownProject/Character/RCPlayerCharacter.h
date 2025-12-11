// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "InputAction.h"

#include "RCPlayerCharacter.generated.h"

class UInputAction;
class ATopDownWeaponBase;

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
	
# pragma region Components
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;
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
	TObjectPtr<UInputAction> InteractFAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FlappingMontage;

private:
	void HandleMoveInput(const FInputActionValue& InValue);
	void HandleDashInput(const FInputActionValue& InValue);
	void HandleInteractFInput(const FInputActionValue& InValue);

# pragma endregion

# pragma region Ablilty

private:
	const float DefaultMaxWalkSpeed = 600.0f;
	const float SprintMaxWalkSpeed = 800.0f;
	const float DashMaxWalkSpeed = 3000.0f;

	const float DashCoolDown = 5.0f;
	
private:
	FVector CurMoveDirection;
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

