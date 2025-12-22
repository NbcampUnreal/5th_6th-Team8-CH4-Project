#include "Weapon/TopDownWeaponBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Weapon/BulletBase.h"

ATopDownWeaponBase::ATopDownWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

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

	if (HasAuthority())
	{
		CurrentAmmoInMag = WeaponStats.MagazineSize;
		bIsReloading = false;
	}
}

void ATopDownWeaponBase::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATopDownWeaponBase, CurrentAmmoInMag);
	DOREPLIFETIME(ATopDownWeaponBase, bIsReloading);
}


void ATopDownWeaponBase::StartFire()
{
	bWantsToFire = true;

	APlayerController* PC = Cast<APlayerController>(GetInstigatorController());
	if (!PC)
		return;

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		return;

	FVector TargetPos = Hit.ImpactPoint;
	TargetPos.Z += 80.f; 
	
	if (!HasAuthority())
	{
		Server_StartFire(TargetPos);
		return;
	}

	CachedTargetWorldPos = TargetPos;
	Server_StartFire(TargetPos);
}

void ATopDownWeaponBase::StopFire()
{
	bWantsToFire = false;

	if (!HasAuthority())
	{
		Server_StopFire();
		return;
	}

	Server_StopFire();
}

void ATopDownWeaponBase::Server_StartFire_Implementation(const FVector_NetQuantize& TargetWorldPos)
{
	CachedTargetWorldPos = TargetWorldPos;
	
	if (bIsReloading)
	{
		bWantsToFire = true;
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(FireTimerHandle))
	{
		return;
	}

	bWantsToFire = true;

	Server_FireOnce();

	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ATopDownWeaponBase::Server_FireOnce,
		WeaponStats.FireInterval,
		true
	);
}

void ATopDownWeaponBase::Server_StopFire_Implementation()
{
	bWantsToFire = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

bool ATopDownWeaponBase::CanFire() const
{
	if (!GetWorld())
	{
		return false;
	}
	if (bIsReloading)
	{
		return false;
	}
	if (CurrentAmmoInMag <= 0)
	{
		return false;
	}

	return (GetWorld()->GetTimeSeconds() - LastFireTime) >= WeaponStats.FireInterval;
}

void ATopDownWeaponBase::Server_FireOnce()
{
	if (!HasAuthority() || !bWantsToFire || !CanFire())
	{
		return;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();

	CurrentAmmoInMag = FMath::Max(0, CurrentAmmoInMag - 1);

	SpawnBullet_Server();

	const FVector Loc = Muzzle ? Muzzle->GetComponentLocation() : GetActorLocation();
	const FRotator Rot = Muzzle ? Muzzle->GetComponentRotation() : GetActorRotation();
	Multicast_PlayFireFX(Loc, Rot);

	if (CurrentAmmoInMag <= 0)
	{
		Server_StartReload();
	}
}

void ATopDownWeaponBase::SpawnBullet_Server()
{
	UE_LOG(LogTemp, Warning,
	       TEXT("[Weapon][Server][SpawnBullet] Authority=%d World=%d BulletClass=%s Muzzle=%s Owner=%s"),
	       HasAuthority(),
	       GetWorld() != nullptr,
	       *GetNameSafe(BulletClass),
	       *GetNameSafe(Muzzle),
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

	const FVector SpawnLoc = Muzzle->GetComponentLocation() + Muzzle->GetForwardVector() * MuzzleOffset;
	FVector FlatTarget = CachedTargetWorldPos;
	FlatTarget.Z = SpawnLoc.Z;        

	FVector Dir = (FlatTarget - SpawnLoc).GetSafeNormal();

	Dir = FMath::VRandCone(
		Dir,
		FMath::DegreesToRadians(WeaponStats.Spread)
	);


	AActor* PooledActor = Pool->SpawnFromPool(
		BulletClass,
		SpawnLoc,
		Dir.Rotation(),
		GetOwner(),
		Cast<APawn>(GetOwner())
	);

	UE_LOG(LogTemp, Warning,
	       TEXT("[Weapon][Server][SpawnBullet] PooledActor=%s"),
	       *GetNameSafe(PooledActor)
	);

	ABulletBase* Bullet = Cast<ABulletBase>(PooledActor);
	if (!Bullet)
	{
		UE_LOG(LogTemp, Error, TEXT("[Weapon][Server][SpawnBullet] Cast to Bullet FAILED"));
		return;
	}

	UE_LOG(LogTemp, Warning,
	       TEXT("[Weapon][Server][SpawnBullet] InitBullet Dir=%s Speed=%.1f Damage=%.1f"),
	       *Dir.ToString(),
	       WeaponStats.BulletSpeed,
	       WeaponStats.Damage
	);

	AController* InstCtrl = nullptr;
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		InstCtrl = OwnerPawn->GetController();
	}

	Bullet->InitBullet(Dir, WeaponStats.BulletSpeed, WeaponStats.Damage, InstCtrl);
}

void ATopDownWeaponBase::Multicast_PlayFireFX_Implementation(const FVector& Loc, const FRotator& Rot)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (MuzzleFlashFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, Loc, Rot);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, Loc);
	}
}

void ATopDownWeaponBase::Server_UpdateAim_Implementation(const FVector_NetQuantize& NewTargetWorldPos)
{
	CachedTargetWorldPos = NewTargetWorldPos;
}

void ATopDownWeaponBase::StartReload()
{
	if (!HasAuthority())
	{
		Server_StartReload();
		return;
	}
}

bool ATopDownWeaponBase::CanReload() const
{
	if (!HasAuthority())
	{
		return false;
	}
	if (bIsReloading)
	{
		return false;
	}
	if (CurrentAmmoInMag >= WeaponStats.MagazineSize)
	{
		return false;
	}

	return true;
}

void ATopDownWeaponBase::Server_StartReload_Implementation()
{
	if (!CanReload())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(FireTimerHandle);

	bIsReloading = true;
	ForceNetUpdate();

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&ATopDownWeaponBase::FinishReload_Server,
		WeaponStats.ReloadDuration,
		false
	);
}

void ATopDownWeaponBase::FinishReload_Server()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentAmmoInMag = WeaponStats.MagazineSize;
	bIsReloading = false;
	ForceNetUpdate();

	if (bWantsToFire)
	{
		Server_StartFire(CachedTargetWorldPos);
	}
}

void ATopDownWeaponBase::OnRep_Ammo()
{
	UE_LOG(LogTemp, Verbose, TEXT("[Weapon][Client] Ammo=%d/%d"),
	       CurrentAmmoInMag, WeaponStats.MagazineSize);
}

void ATopDownWeaponBase::OnRep_Reloading()
{
	if (bIsReloading && ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ReloadSound,
			GetActorLocation()
		);
	}
}
