#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorObjectPoolSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPool, Log, All);

USTRUCT()
struct FActorPoolBucket
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> InactiveObjects;

	bool IsEmpty() const { return InactiveObjects.IsEmpty(); }

	AActor* Pop()
	{
		return InactiveObjects.Pop(EAllowShrinking::No);
	}

	void Add(AActor* Actor)
	{
		InactiveObjects.Add(Actor);
	}
};

UCLASS()
class CH4TOPDOWNPROJECT_API UActorObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void InitializePool(TSubclassOf<AActor> ActorClass, int32 MinCount);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	AActor* SpawnFromPool(TSubclassOf<AActor> ActorClass,
		const FVector& Location,
		const FRotator& Rotation,
		AActor* Owner = nullptr,
		APawn* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void ReturnToPool(AActor* Actor);

private:
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FActorPoolBucket> Pools;

	AActor* SpawnNew(TSubclassOf<AActor> ActorClass,
		const FVector& Location,
		const FRotator& Rotation,
		AActor* Owner,
		APawn* Instigator);
};
