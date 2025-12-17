#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Poolable.generated.h"

UINTERFACE(BlueprintType)
class CH4TOPDOWNPROJECT_API UPoolable : public UInterface
{
	GENERATED_BODY()
};


class CH4TOPDOWNPROJECT_API IPoolable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	void OnSpawnFromPool();

	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	void OnReturnToPool();

	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	UClass* GetPoolKeyClass();
};
