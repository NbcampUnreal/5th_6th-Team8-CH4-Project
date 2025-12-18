#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopDownWeaponBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UNiagaraSystem;
class USoundBase;
class ABulletBase;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol,
	Rifle,
	Sniper,
	Shotgun,
};

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireInterval = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BulletSpeed = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Spread = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float MaxRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float ReloadDuration = 1.6f;
};

UCLASS()
class CH4TOPDOWNPROJECT_API ATopDownWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ATopDownWeaponBase();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartFire();
	virtual void StopFire();

	UFUNCTION(Server, Reliable)
	void Server_StartFire();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();

	void StartReload();

	UFUNCTION(Server, Reliable)
	void Server_StartReload();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> MagazineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> Muzzle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::Rifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FWeaponStats WeaponStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float MuzzleOffset = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Bullet")
	TSubclassOf<ABulletBase> BulletClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TObjectPtr<UNiagaraSystem> MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|SFX")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|SFX")
	TObjectPtr<USoundBase> ReloadSound;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayFireFX(const FVector& Loc, const FRotator& Rot);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmoInMag = 0;

	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(ReplicatedUsing = OnRep_Reloading, BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bIsReloading = false;

	UFUNCTION()
	void OnRep_Reloading();

private:
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	bool bWantsToFire = false;
	float LastFireTime = -FLT_MAX;

	void Server_FireOnce();
	bool CanFire() const;

	void SpawnBullet_Server();

	bool CanReload() const;
	void FinishReload_Server();
};
