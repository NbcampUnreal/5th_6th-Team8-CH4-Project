#include "Weapon/ShotgunWeapon.h"

#include "Engine/World.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Weapon/BulletBase.h"

AShotgunWeapon::AShotgunWeapon()
{
	WeaponType = EWeaponType::Shotgun;

	WeaponStats.Damage = 60.f;
	WeaponStats.FireInterval = 1.0f;
	WeaponStats.BulletSpeed = 2000.f;
	WeaponStats.Spread = 2.5f;
	WeaponStats.MagazineSize = 6;
	WeaponStats.ReloadDuration = 2.2f;

	PelletsPerShot = 8;
	PelletDamageScale = 1.0f / PelletsPerShot;
	ExtraPelletSpreadDeg = 4.0f;
}

FVector AShotgunWeapon::ComputeBulletDirection_Server(const FVector& SpawnLoc) const
{
	FVector FlatTarget = CachedTargetWorldPos;
	FlatTarget.Z = SpawnLoc.Z;

	FVector Dir = (FlatTarget - SpawnLoc).GetSafeNormal();

	const float TotalSpreadRad =
		FMath::DegreesToRadians(WeaponStats.Spread + ExtraPelletSpreadDeg);

	return FMath::VRandCone(Dir, TotalSpreadRad);
}

void AShotgunWeapon::SpawnBullet_Server()
{
	UE_LOG(LogTemp, Warning,
		TEXT("[Shotgun][Server] World=%d BulletClass=%s Muzzle=%s Pool=%s Owner=%s"),
		GetWorld() != nullptr,
		*GetNameSafe(BulletClass),
		*GetNameSafe(Muzzle),
		*GetNameSafe(GetWorld() ? GetWorld()->GetSubsystem<UActorObjectPoolSubsystem>() : nullptr),
		*GetNameSafe(GetOwner())
	);

	if (!HasAuthority() || !GetWorld() || !BulletClass || !Muzzle)
	{
		return;
	}

	UActorObjectPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorObjectPoolSubsystem>();
	if (!Pool)
	{
		return;
	}

	AController* InstCtrl = nullptr;
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		InstCtrl = OwnerPawn->GetController();
	}

	const FVector SpawnLoc = Muzzle->GetComponentLocation() + Muzzle->GetForwardVector() * MuzzleOffset;

	for (int32 i = 0; i < PelletsPerShot; ++i)
	{
		const FVector Dir = ComputeBulletDirection_Server(SpawnLoc);

		AActor* PooledActor = Pool->SpawnFromPool(
			BulletClass,
			SpawnLoc,
			Dir.Rotation(),
			GetOwner(),
			Cast<APawn>(GetOwner())
		);

		ABulletBase* Bullet = Cast<ABulletBase>(PooledActor);
		if (!Bullet)
		{
			continue;
		}

		const float PelletDamage = WeaponStats.Damage * PelletDamageScale;
		Bullet->InitBullet(Dir, WeaponStats.BulletSpeed, PelletDamage, InstCtrl,WeaponStats.MaxRange);
	}
}