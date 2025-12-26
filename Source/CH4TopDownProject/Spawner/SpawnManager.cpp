
#include "Spawner/SpawnManager.h"
#include "Spawner/SpawnPoint.h"
#include "Kismet/GameplayStatics.h"

ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASpawnManager::SpawnCharacter(AActor* OtherActor)
{
	ASpawnPoint* CharacterSpawnPoint = Cast<ASpawnPoint>(SpawnPointList[SpawnCount]);

	OtherActor->SetActorLocation(CharacterSpawnPoint->GetActorLocation());
	
	if (SpawnCount < SpawnPointList.Num() - 1)
	{
		++SpawnCount;
	}
	else
	{
		SpawnCount = SpawnCount - (SpawnPointList.Num() - 1);
	}
}

void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), SpawnPointList);
	}
	
}

