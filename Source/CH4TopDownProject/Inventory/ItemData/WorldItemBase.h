// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "WorldItemBase.generated.h"

class UWidgetComponent;
class UBoxComponent;

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

	UPROPERTY(VisibleAnywhere, Category="Interact")
	TObjectPtr<UWidgetComponent> InteractWidget;
	
	UPROPERTY(EditAnywhere)
	bool bCanInteract =true;

	UPROPERTY(EditAnywhere)
	bool bCanTakeDamage=true;

	UPROPERTY(EditAnywhere)
	float MaxHP =100;

	UPROPERTY(EditAnywhere)
	float CurrentHP;


	UFUNCTION(BlueprintCallable)
	virtual void HandleDestroyed();

	UFUNCTION(BlueprintNativeEvent)
	 void OnItemDestroyed();

	UFUNCTION(BlueprintCallable)
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	virtual  void NotifyActorEndOverlap(AActor* OtherActor);

public :
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void TakeDamage_Implementation(float Damage, AActor* DamageCauser) override;
	
	virtual void ShowInteractWidget(AActor* OtherActor);
	virtual void HideInteractWidget(AActor* OtherActor);

	
};
