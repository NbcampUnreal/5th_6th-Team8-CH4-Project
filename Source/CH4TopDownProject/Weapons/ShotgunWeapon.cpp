// Weapons/ShotgunWeapon.cpp
#include "Weapons/ShotgunWeapon.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Weapon/BulletBase.h"
#include "GameFramework/Pawn.h"

AShotgunWeapon::AShotgunWeapon()
{
    WeaponType = EWeaponTypes::Range;

    CommonStats.Damage = 60.f;
    CommonStats.AttackInterval = 1.0f;
    CommonStats.bAutoRepeat = true;

    RangeStats.BulletSpeed = 2000.f;
    RangeStats.SpreadDeg = 2.5f;
    RangeStats.MagazineSize = 6;
    RangeStats.ReloadDuration = 2.2f;
    RangeStats.MaxRange = 2000.f;

    PelletsPerShot = 8;
    PelletDamageScale = 1.0f / FMath::Max(1, PelletsPerShot);
    ExtraPelletSpreadDeg = 4.0f;
}

bool AShotgunWeapon::Server_AttackOnce()
{
    if (!HasAuthority() || !GetWorld())
    {
        return false;
    }

    if (bIsReloading)
    {
        return false;
    }

    if (CurrentAmmo <= 0)
    {
        StartReload();
        return false;
    }

    CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);

    UActorObjectPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorObjectPoolSubsystem>();
    if (!Pool || !BulletClass || !Muzzle)
    {
        return false;
    }

    AController* InstCtrl = nullptr;
    if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
    {
        InstCtrl = PawnOwner->GetController();
    }

    const FVector SpawnLoc =
        Muzzle->GetComponentLocation() + Muzzle->GetForwardVector() * RangeStats.MuzzleOffset;

    for (int32 i = 0; i < PelletsPerShot; ++i)
    {
        FVector Target = CachedTargetWorldPos;
        Target.Z = SpawnLoc.Z;

        FVector Dir = (Target - SpawnLoc).GetSafeNormal();

        const float TotalSpreadRad =
            FMath::DegreesToRadians(RangeStats.SpreadDeg + ExtraPelletSpreadDeg);

        Dir = FMath::VRandCone(Dir, TotalSpreadRad);

        AActor* Spawned = Pool->SpawnFromPool(
            BulletClass,
            SpawnLoc,
            Dir.Rotation(),
            GetOwner(),
            Cast<APawn>(GetOwner())
        );

        ABulletBase* Bullet = Cast<ABulletBase>(Spawned);
        if (!Bullet)
        {
            continue;
        }

        const float PelletDamage = CommonStats.Damage * PelletDamageScale;

        Bullet->InitBullet(
            Dir,
            RangeStats.BulletSpeed,
            PelletDamage,
            InstCtrl,
            RangeStats.MaxRange
        );
    }

    if (CurrentAmmo <= 0)
    {
        StartReload();
    }

    ForceNetUpdate();
    return true;
}
