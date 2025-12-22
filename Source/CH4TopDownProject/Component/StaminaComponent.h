#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangeSignature, float, NewStamina, float, MaxStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH4TOPDOWNPROJECT_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();	

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnRep_CurrentStamina(float OldStamina);

	void SetStamina(float NewStamina);

public:	
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChangeSignature OnStaminaChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartStaminaDrain(float DrainRate);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopStaminaDrain();

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina() const;

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStamina, VisibleAnywhere, Category = "Stamina")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "1.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRecoveryRate;

	float CurrentDrainRate;
};