#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlueZoneActor.generated.h"

enum class EBlueZoneState : uint8
{
	Waiting,
	Shrinking,
	Finished,
};

UCLASS()
class CH4TOPDOWNPROJECT_API ABlueZoneActor : public AActor
{
	GENERATED_BODY()

public:
	ABlueZoneActor();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ApplyBlueZoneDamage();

	void StartShrink();

	void UpdateShrink();

	void DrawDebugBlueZone();

public:
	void ActivateBlueZone();

protected:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(EditAnywhere, Category = "BlueZone")
	float WaitTime = 180.0f;

	UPROPERTY(EditAnywhere, Category = "BlueZone")
	float ShrinkDuration = 200.0f;

	UPROPERTY(EditAnywhere, Category = "BlueZone")
	float BlueZoneRadius = 33000.0f;

	UPROPERTY(EditAnywhere, Category = "BlueZone")
	float TargetRadius = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "BlueZone")
	float BlueZoneDamage = 5.0f;

	UPROPERTY(EditAnywhere, Category = "BlueZone")
	float DamageInterval = 1.0f;
		
	UPROPERTY(Replicated)
	float CurrentRadius;

	float ShrinkElapsedTime = 0.0f;	

	EBlueZoneState BlueZoneState = EBlueZoneState::Waiting;
		
	FTimerHandle BlueZoneDamageHandle;

	FTimerHandle ShrinkTimerHandle;
};