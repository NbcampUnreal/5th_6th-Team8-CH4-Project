#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTextActor.generated.h"

class UWidgetComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API ADamageTextActor : public AActor
{
	GENERATED_BODY()
	
public:		
	ADamageTextActor();

	void InitializeDamage(float Damage);

protected:	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> DamageWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float LifeSpan = 1.0f;
};
