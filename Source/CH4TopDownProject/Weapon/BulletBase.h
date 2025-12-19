#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Poolable.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "BulletBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class CH4TOPDOWNPROJECT_API ABulletBase : public AActor, public IPoolable
{
	GENERATED_BODY()

public:
	ABulletBase();

	void InitBullet(
		const FVector& Dir,
		float Speed,
		float InDamage,
		AController* InInstigatorController
	);

	void ReturnToPool();

protected:
	virtual void BeginPlay() override;

	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
	virtual UClass* GetPoolKeyClass_Implementation() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
	TObjectPtr<UStaticMeshComponent> BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|VFX")
	TObjectPtr<UNiagaraComponent> TrailComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet|VFX")
	TObjectPtr<UNiagaraSystem> TrailFX;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float LifeTime = 3.f;

private:
	FTimerHandle LifeTimer;

	float Damage = 0.f;
	TWeakObjectPtr<AController> InstigatorController;

	void StartLifeTimer();
	void StopLifeTimer();

private:
	void ReturnToPool_Internal(const TCHAR* Reason);
	void ReturnToPool_FromLifeTime();
};
