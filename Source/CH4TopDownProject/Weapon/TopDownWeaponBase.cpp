// TopDownWeaponBase.cpp

#include "TopDownWeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

ATopDownWeaponBase::ATopDownWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(WeaponMesh);

    MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
    MagazineMesh->SetupAttachment(WeaponMesh);

    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    WeaponMesh->SetSimulatePhysics(false);

    MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MagazineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    MagazineMesh->SetSimulatePhysics(false);
}


void ATopDownWeaponBase::BeginPlay()
{
    Super::BeginPlay();
}

void ATopDownWeaponBase::StartFire()
{
    Fire();

    GetWorldTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &ATopDownWeaponBase::Fire,
        WeaponStats.FireInterval,
        true
    );
}

void ATopDownWeaponBase::StopFire()
{
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ATopDownWeaponBase::Fire()
{
    PerformLineTrace();

    PlayMuzzleFlash();

    PlayFireSound();
}

void ATopDownWeaponBase::PerformLineTrace()
{
    if (!Muzzle || !GetWorld()) return;

    const FVector Start = Muzzle->GetComponentLocation();

    FVector ShootDir = Muzzle->GetComponentRotation().Vector();

    ShootDir = FMath::VRandCone(ShootDir, FMath::DegreesToRadians(WeaponStats.Spread));

    const FVector End = Start + ShootDir * WeaponStats.MaxRange;
    
    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        FColor::Red,
        false,
        1.0f,
        0,
        2.0f
    );

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    if (bHit)
    {
        UGameplayStatics::ApplyPointDamage(
            Hit.GetActor(),
            WeaponStats.Damage,
            ShootDir,
            Hit,
            GetInstigatorController(),
            this,
            nullptr
        );
    }
}

void ATopDownWeaponBase::PlayMuzzleFlash()
{
    if (!Muzzle || !MuzzleFlashFX || !GetWorld())
        return;

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        MuzzleFlashFX,
        Muzzle->GetComponentLocation(),
        Muzzle->GetComponentRotation()
    );
}

void ATopDownWeaponBase::PlayFireSound()
{
    if (!FireSound || !GetWorld())
        return;

    UGameplayStatics::PlaySoundAtLocation(
        GetWorld(),
        FireSound,
        Muzzle ? Muzzle->GetComponentLocation() : GetActorLocation()
    );
}