#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Engine/World.h"
#include "Interface/Poolable.h"

DEFINE_LOG_CATEGORY(LogPool);

void UActorObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogPool, Log, TEXT("ActorObjectPoolSubsystem Initialized"));
}

void UActorObjectPoolSubsystem::Deinitialize()
{
	Pools.Empty();
	UE_LOG(LogPool, Log, TEXT("ActorObjectPoolSubsystem Deinitialized"));
	Super::Deinitialize();
}

void UActorObjectPoolSubsystem::InitializePool(TSubclassOf<AActor> ActorClass, int32 MinCount)
{
	if (!GetWorld() || !*ActorClass || MinCount <= 0) return;

	FActorPoolBucket& Bucket = Pools.FindOrAdd(ActorClass);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	while (Bucket.InactiveObjects.Num() < MinCount)
	{
		AActor* A = GetWorld()->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (!A) break;

		A->SetActorHiddenInGame(true);
		A->SetActorEnableCollision(false);
		A->SetActorTickEnabled(false);

		if (A->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
		{
			IPoolable::Execute_OnReturnToPool(A);
		}

		Bucket.Add(A);
	}

	UE_LOG(LogPool, Log, TEXT("[INIT] %s Inactive=%d"), *GetNameSafe(*ActorClass), Bucket.InactiveObjects.Num());
}

AActor* UActorObjectPoolSubsystem::SpawnFromPool(
	TSubclassOf<AActor> ActorClass,
	const FVector& Location,
	const FRotator& Rotation,
	AActor* Owner,
	APawn* Instigator)
{
	if (!GetWorld() || !*ActorClass) return nullptr;

	FActorPoolBucket& Bucket = Pools.FindOrAdd(ActorClass);

	AActor* A = nullptr;
	if (!Bucket.IsEmpty())
	{
		A = Bucket.Pop();
	}
	else
	{
		A = SpawnNew(ActorClass, Location, Rotation, Owner, Instigator);
	}

	if (!A) return nullptr;

	TSubclassOf<AActor> KeyClass = A->GetClass();
	if (A->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		if (UClass* OverrideKey = IPoolable::Execute_GetPoolKeyClass(A))
		{
			KeyClass = OverrideKey;
		}
	}

	A->SetOwner(Owner);
	if (Instigator) A->SetInstigator(Instigator);

	A->SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
	A->SetActorHiddenInGame(false);
	A->SetActorEnableCollision(true);
	A->SetActorTickEnabled(true);

	if (A->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnSpawnFromPool(A);
	}

	return A;
}

void UActorObjectPoolSubsystem::ReturnToPool(AActor* Actor)
{
	if (!Actor) return;

	TSubclassOf<AActor> KeyClass = Actor->GetClass();
	if (Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(Actor);
		if (UClass* OverrideKey = IPoolable::Execute_GetPoolKeyClass(Actor))
		{
			KeyClass = OverrideKey;
		}
	}

	Actor->SetActorEnableCollision(false);
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);

	FActorPoolBucket& Bucket = Pools.FindOrAdd(KeyClass);
	Bucket.Add(Actor);
}

AActor* UActorObjectPoolSubsystem::SpawnNew(TSubclassOf<AActor> ActorClass,
	const FVector& Location,
	const FRotator& Rotation,
	AActor* Owner,
	APawn* Instigator)
{
	if (!GetWorld() || !*ActorClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = Owner;
	Params.Instigator = Instigator;

	return GetWorld()->SpawnActor<AActor>(ActorClass, Location, Rotation, Params);
}
