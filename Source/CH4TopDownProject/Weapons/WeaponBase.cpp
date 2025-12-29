// Weapons/WeaponBase.cpp
#include "Weapons/WeaponBase.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Character/RCPlayerCharacter.h"
#include "Inventory/InventoryComponent.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    //WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    //RootComponent = WeaponMesh;
    WeaponMesh = StaticMesh;

    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    WeaponMesh->SetSimulatePhysics(false);
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeaponBase::StartAttack(const FVector_NetQuantize& TargetWorldPos)
{
    if (!HasAuthority())
    {
        Server_StartAttack(TargetWorldPos);
        return;
    }
    Server_StartAttack(TargetWorldPos);
}

void AWeaponBase::StopAttack()
{
    if (!HasAuthority())
    {
        Server_StopAttack();
        return;
    }
    Server_StopAttack();
}

void AWeaponBase::Server_StartAttack_Implementation(const FVector_NetQuantize& TargetWorldPos)
{
    CachedTargetWorldPos = TargetWorldPos;
    bWantsToAttack = true;

    if (GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
    {
        return;
    }

    Server_AttackTick();

    if (CommonStats.bAutoRepeat)
    {
        GetWorldTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &AWeaponBase::Server_AttackTick,
            GetAttackInterval(),
            true
        );
    }
}

void AWeaponBase::Server_StopAttack_Implementation()
{
    bWantsToAttack = false;
    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void AWeaponBase::Server_UpdateAim_Implementation(const FVector_NetQuantize& NewTargetWorldPos)
{
    CachedTargetWorldPos = NewTargetWorldPos;
}

bool AWeaponBase::CanAttack() const
{
    if (!HasAuthority() || !GetWorld()) return false;
    if (!bWantsToAttack) return false;

    const float Interval = GetAttackInterval();
    return (GetWorld()->GetTimeSeconds() - LastAttackTime) >= Interval;
}

void AWeaponBase::Server_AttackTick()
{
    if (!CanAttack())
    {
        return;
    }

    LastAttackTime = GetWorld()->GetTimeSeconds();

    const bool bDidAttack = Server_AttackOnce();
    if (bDidAttack)
    {
        const FVector Loc = WeaponMesh ? WeaponMesh->GetComponentLocation() : GetActorLocation();
        const FRotator Rot = WeaponMesh ? WeaponMesh->GetComponentRotation() : GetActorRotation();
        Multicast_PlayAttackFX(Loc, Rot);
    }

    if (!CommonStats.bAutoRepeat)
    {
        bWantsToAttack = false;
        GetWorldTimerManager().ClearTimer(AttackTimerHandle);
    }
}

void AWeaponBase::Multicast_PlayAttackFX_Implementation(const FVector& Loc, const FRotator& Rot)
{
    if (GetNetMode() == NM_DedicatedServer) return;

    if (AttackFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackFX, Loc, Rot);
    }
    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, Loc);
    }
}

void AWeaponBase::Interact_Implementation(AActor* Interactor)
{
    if (!HasAuthority())
    {
        return;
    }

    ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(Interactor);
    if (Player)
    {
        Player->GetInventoryComponent()->GetItem(this);
        
		this->Destroy();
    }
}
