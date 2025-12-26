#include "Weapon/MeleeWeapon.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"

AMeleeWeapon::AMeleeWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponType = EWeaponType::Melee;

    WeaponStats.MagazineSize = 0;
    CurrentAmmoInMag = 0;

    SwingPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SwingPivot"));
    SetRootComponent(SwingPivot);

    WeaponMesh->SetupAttachment(SwingPivot);
}

bool AMeleeWeapon::IsInFrontArc(const FVector& OwnerForward, const FVector& ToTarget) const
{
    const float CosHalf = FMath::Cos(FMath::DegreesToRadians(MeleeStats.AngleDeg * 0.5f));
    const float Dot = FVector::DotProduct(OwnerForward.GetSafeNormal2D(), ToTarget.GetSafeNormal2D());
    return Dot >= CosHalf;
}

void AMeleeWeapon::Multicast_PlaySwingFX_Implementation()
{
    if (GetNetMode() == NM_DedicatedServer)
    { 
        return;
    }

    if (CanPlaySwingVisual())
    {
        LastSwingVisualTime = GetWorld()->GetTimeSeconds();
        BeginSwingVisual();
    }

    if (!SwingFX || !WeaponMesh)
    {
        return;
    }

    UNiagaraFunctionLibrary::SpawnSystemAttached(
        SwingFX,
        WeaponMesh,
        SwingSocketName,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );
}

void AMeleeWeapon::Server_AttackOnce()
{
    if (!HasAuthority() || !GetWorld())
    {
        return;
    }

    Multicast_PlaySwingFX();

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn)
    {
        return;
    }
    const FVector Start = OwnerPawn->GetActorLocation();
    const FVector Forward = OwnerPawn->GetActorForwardVector();
    const FVector End = Start + Forward * MeleeStats.Range;

    TArray<FHitResult> Hits;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MeleeAttack), false);
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(OwnerPawn);

    const bool bHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(MeleeStats.Radius),
        Params
    );

#if !(UE_BUILD_SHIPPING)
    DrawDebugSphere(GetWorld(), End, MeleeStats.Radius, 12, bHit ? FColor::Red : FColor::Green, false, 0.3f);
#endif

    if (!bHit)
    {
        return;
    }
    AController* InstCtrl = OwnerPawn->GetController();

    TSet<TWeakObjectPtr<AActor>> Damaged;

    for (const FHitResult& HR : Hits)
    {
        AActor* Victim = HR.GetActor();
        if (!Victim || Victim == OwnerPawn)
        {
            continue;
        }
        if (Damaged.Contains(Victim))
        {
            continue;
        }
        const FVector ToTarget = Victim->GetActorLocation() - OwnerPawn->GetActorLocation();
        if (!IsInFrontArc(Forward, ToTarget))
        {
            continue;

        }
        Damaged.Add(Victim);

        UGameplayStatics::ApplyPointDamage(
            Victim,
            MeleeStats.Damage,
            Forward,
            HR,
            InstCtrl,
            this,
            nullptr
        );
    }
}

float AMeleeWeapon::GetAttackInterval() const
{
    return MeleeStats.AttackInterval;
}

void AMeleeWeapon::StartFire()
{
    if (CanPlaySwingVisual())
    {
        LastSwingVisualTime = GetWorld()->GetTimeSeconds();
        BeginSwingVisual();
    }

    Super::StartFire();
}

void AMeleeWeapon::BeginSwingVisual()
{
    if (!SwingPivot || !GetWorld()) 
    {
        return;
    }

    if (!bSwinging)
    {
        CachedPivotRot = SwingPivot->GetRelativeRotation();
    }

    bSwinging = true;
    SwingStartTime = GetWorld()->GetTimeSeconds();
}

void AMeleeWeapon::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bSwinging || !SwingPivot || !GetWorld()) 
    {
        return;
    }

    const float t = GetWorld()->GetTimeSeconds() - SwingStartTime;

    if (t >= SwingDuration * 2)
    {
        SwingPivot->SetRelativeRotation(CachedPivotRot + SwingRotA);
        bSwinging = false;
        return;
    }

    float Alpha = 0.f;

    if (t <= SwingDuration)
    {
        Alpha = t / SwingDuration;
        Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.0f);
    }
    else
    {
        const float rt = (t - SwingDuration) / SwingDuration;
        float Ease = FMath::InterpEaseInOut(0.f, 1.f, rt, 2.0f);
        Alpha = 1.f - Ease;
    }

    const FRotator AddRot = FMath::Lerp(SwingRotA, SwingRotB, Alpha);
    SwingPivot->SetRelativeRotation(CachedPivotRot + AddRot);
}

bool AMeleeWeapon::CanPlaySwingVisual() const
{
    if (!GetWorld()) 
    {
        return false;
    }

    if (bSwinging) 
    {
        return false;
    }

    const float Now = GetWorld()->GetTimeSeconds();
    const float Interval = GetAttackInterval();
    return (Now - LastSwingVisualTime) >= Interval;
}