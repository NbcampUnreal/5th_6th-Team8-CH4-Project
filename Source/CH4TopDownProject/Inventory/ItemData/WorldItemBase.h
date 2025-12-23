// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "WorldItemBase.generated.h"

class UWidgetComponent;
class UBoxComponent;
class USphereComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API AWorldItemBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AWorldItemBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Collision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> InteractionCheckCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Interact")
	TObjectPtr<UWidgetComponent> InteractWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	TObjectPtr<UWidgetComponent> InteractCheckWidget;

	UPROPERTY(EditAnywhere)
	bool bCanInteract = true;

	UPROPERTY(EditAnywhere)
	bool bCanTakeDamage = true;

	UPROPERTY(EditAnywhere)
	float MaxHP = 100;

	UPROPERTY(EditAnywhere)
	float CurrentHP;

	UPROPERTY(EditAnywhere)
	float CollisionRadius = 700.0f;

	UFUNCTION(BlueprintCallable)
	virtual void HandleDestroyed();

	UFUNCTION(BlueprintNativeEvent)
	void OnItemDestroyed();

	UFUNCTION(BlueprintCallable)
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnSphereEnd(UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	virtual void OnCheckOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnCheckEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public :
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void TakeDamage_Implementation(float Damage, AActor* DamageCauser) override;

	virtual void ShowInteractWidget(bool bVisible);
	virtual void ShowInteractCheckWidget(bool bVisible);

	virtual void SetOutLineEnable(bool Enable);
	virtual void SetPreviewWidgetEnable(bool Enable);
};
