
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

class ASpawnPoint;

UCLASS()
class CH4TOPDOWNPROJECT_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnManager();
	
	void SpawnCharacter(AActor* OtherActor);

protected:
	virtual void BeginPlay() override;

public:
	TArray<AActor*> SpawnPointList;
};
