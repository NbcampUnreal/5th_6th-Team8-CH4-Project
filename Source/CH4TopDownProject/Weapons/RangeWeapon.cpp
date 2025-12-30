// Weapons/RangeWeapon.cpp
#include "Weapons/RangeWeapon.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Weapon/BulletBase.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

ARangeWeapon::ARangeWeapon()
{
    WeaponType = EWeaponTypes::Range;

    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(WeaponMesh);

    MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
    MagazineMesh->SetupAttachment(WeaponMesh);

    MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MagazineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    MagazineMesh->SetSimulatePhysics(false);

    bReplicates = true;
    SetReplicateMovement(true);
}

void ARangeWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        CurrentAmmo = RangeStats.MagazineSize;
        bIsReloading = false;
        PendingReloadFill = 0;
    }
}

void ARangeWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 

    if (!HasAuthority())
        return;

    if (!bReloadRequested)
        return;

    if (bIsReloading)
        return;

    if (!GetWorld())
        return;

    const float Now = GetWorld()->GetTimeSeconds();
    const float Interval = GetAttackInterval();

    if ((Now - LastAttackTime) < Interval)
        return;

    bReloadRequested = false;
    StartReload_Internal();
}

void ARangeWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (HasAuthority())
    {
        GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
        PendingReloadFill = 0;
        bIsReloading = false;
    }

    Super::EndPlay(EndPlayReason);
}

float ARangeWeapon::GetAttackInterval() const
{
    return CommonStats.AttackInterval;
}

bool ARangeWeapon::CanShoot() const
{
    if (!HasAuthority()) return false;
    if (bIsReloading) return false;
    if (CurrentAmmo <= 0) return false;
    return true;
}

UInventoryComponent* ARangeWeapon::GetOwnerInventory_Server() const
{
    if (!HasAuthority()) return nullptr;

    const APawn* PawnOwner = Cast<APawn>(GetOwner());
    if (!PawnOwner) return nullptr;

    return PawnOwner->FindComponentByClass<UInventoryComponent>();
}

int32 ARangeWeapon::GetMissingAmmoToFill() const
{
    return FMath::Max(0, RangeStats.MagazineSize - CurrentAmmo);
}

bool ARangeWeapon::Server_AttackOnce()
{
    if (!HasAuthority())
    {
        return false;
    }

    if (!CanShoot())
    {
        return false;
    }

    bPrevReloading = false;

    CurrentAmmo--;
    SpawnBullet_Server();

    if (CurrentAmmo <= 0)
    {
        bPrevReloading = true;
    }

    ForceNetUpdate();
    return true;
}


void ARangeWeapon::SpawnBullet_Server()
{
    if (!BulletClass || !Muzzle || !GetWorld())
    {
        return;
    }

    UActorObjectPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorObjectPoolSubsystem>();
    if (!Pool)
    {
        return;
    }

    const FVector SpawnLoc =
        Muzzle->GetComponentLocation() + Muzzle->GetForwardVector() * RangeStats.MuzzleOffset;

    const FVector Dir = ComputeBulletDirection_Server(SpawnLoc);

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
        return;
    }

    AController* InstCtrl = nullptr;
    if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
    {
        InstCtrl = PawnOwner->GetController();
    }

    Bullet->InitBullet(
        Dir,
        RangeStats.BulletSpeed,
        CommonStats.Damage,
        InstCtrl,
        RangeStats.MaxRange
    );
}

FVector ARangeWeapon::ComputeBulletDirection_Server(const FVector& SpawnLoc) const
{
    FVector Target = CachedTargetWorldPos;
    Target.Z = SpawnLoc.Z;

    FVector Dir = (Target - SpawnLoc).GetSafeNormal();

    Dir = FMath::VRandCone(
        Dir,
        FMath::DegreesToRadians(RangeStats.SpreadDeg)
    );

    return Dir;
}

void ARangeWeapon::StartReload()
{
    if (!HasAuthority())
    {
        Server_StartReload();
        return;
    }
    if (bIsReloading)
    {
        return;
    }
    bReloadRequested = true;

    bWantsToAttack = false;
    AttackAccum = 0.f;
}

void ARangeWeapon::Server_StartReload_Implementation()
{
    StartReload();
}

bool ARangeWeapon::StartReload_Internal()
{
    if (!HasAuthority() || !GetWorld())
    {
        return false;
    }


    if (bIsReloading)
    {
        return false;
    }

    const int32 Missing = FMath::Max(0, RangeStats.MagazineSize - CurrentAmmo);
    if (Missing <= 0)
    {
        return false;
    }

    UInventoryComponent* Inv = GetOwnerInventory_Server();
    if (!Inv)
    {
        return false;
    }

    const int32 Before = Inv->CheckItem_ID(AmmoItemID);
    if (Before <= 0)
    {
        return false;
    }

    const int32 Need = FMath::Min(Missing, Before);
    Inv->UseItem_ID(AmmoItemID, Need);

    const int32 After = Inv->CheckItem_ID(AmmoItemID);
    const int32 Used = FMath::Clamp(Before - After, 0, Need);
    if (Used <= 0)
    {
        return false;
    }

	PendingReloadFill = Used;
    bIsReloading = true;
    ForceNetUpdate();

    GetWorldTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &ARangeWeapon::FinishReload_Server,
        RangeStats.ReloadDuration,
        false
    );

    return true;
}

void ARangeWeapon::FinishReload_Server()
{
    if (!HasAuthority())
    {
        return;
    }

    CurrentAmmo = FMath::Clamp(CurrentAmmo + PendingReloadFill, 0, RangeStats.MagazineSize);

    PendingReloadFill = 0;
    bIsReloading = false;
    ForceNetUpdate();
}

void ARangeWeapon::OnRep_Ammo()
{
    UE_LOG(LogTemp, Verbose,
        TEXT("[RangeWeapon][Client] Ammo=%d/%d"),
        CurrentAmmo,
        RangeStats.MagazineSize
    );
}

void ARangeWeapon::OnRep_Reloading()
{
    if (GetNetMode() == NM_DedicatedServer) return;

    if (!bPrevReloading && bIsReloading)
    {
        if (ReloadSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                GetWorld(),
                ReloadSound,
                GetActorLocation()
            );
        }
    }
    bPrevReloading = bIsReloading;
}

void ARangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARangeWeapon, CurrentAmmo);
    DOREPLIFETIME(ARangeWeapon, bIsReloading);
}
