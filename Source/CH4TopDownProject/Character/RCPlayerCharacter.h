// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "InputAction.h"

#include "RCPlayerCharacter.generated.h"

class UInputAction;

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
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> IA_Aim;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> IA_InteractF;

private:
	void HandleMoveInput(const FInputActionValue& InValue);
	void HandleAimInput(const FInputActionValue& InValue);
	void HandleInteractFInput(const FInputActionValue& InValue);

# pragma endregion
};
