
#include "RCGameModeBase.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (UActorObjectPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorObjectPoolSubsystem>())
	{
		Pool->InitializePool(BP_Bullet_Pistol_Class, 200);
		Pool->InitializePool(BP_Bullet_Rifle_Class, 300);
		Pool->InitializePool(BP_Bullet_Sniper_Class, 150);
		Pool->InitializePool(BP_Bullet_Shotgun_Class, 250);
	}
}
