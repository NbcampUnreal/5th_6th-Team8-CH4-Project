// Weapons/MeleeWeapon.cpp
#include "Weapons/MeleeWeapon.h"

#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AMeleeWeapon::AMeleeWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponType = EWeaponTypes::Melee;

    SwingPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SwingPivot"));
    SetRootComponent(SwingPivot);

    if (WeaponMesh)
    {
        WeaponMesh->SetupAttachment(SwingPivot);
    }

    SwingPivot->SetMobility(EComponentMobility::Movable);
}

void AMeleeWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (SwingPivot)
    {
        CachedPivotRot = SwingPivot->GetRelativeRotation();
    }
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
    if (!GetWorld())
    {
        return;
    }

    if (CanPlaySwingVisual())
    {
        LastSwingVisualTime = GetWorld()->GetTimeSeconds();
        BeginSwingVisual();
    }


    if (SwingSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            SwingSound,
            GetActorLocation()
        );
    }
}


bool AMeleeWeapon::Server_AttackOnce()
{
    if (!HasAuthority() || !GetWorld())
    {
        return false;
    }

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn)
    {
        return false;
    }

    Multicast_PlaySwingFX();

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
        return false;
    }

    AController* InstCtrl = OwnerPawn->GetController();

    TSet<TWeakObjectPtr<AActor>> Damaged;
    bool bAppliedAnyDamage = false;

    for (const FHitResult& HR : Hits)
    {
        AActor* Victim = HR.GetActor();
        if (!Victim || Victim == OwnerPawn) continue;
        if (Damaged.Contains(Victim)) continue;

        const FVector ToTarget = Victim->GetActorLocation() - OwnerPawn->GetActorLocation();
        if (!IsInFrontArc(Forward, ToTarget)) continue;

        Damaged.Add(Victim);

        UGameplayStatics::ApplyPointDamage(
            Victim,
            CommonStats.Damage,
            Forward,
            HR,
            InstCtrl,
            this,
            nullptr
        );

        bAppliedAnyDamage = true;
    }

    return bAppliedAnyDamage;
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

bool AMeleeWeapon::CanPlaySwingVisual() const
{
    if (!GetWorld()) return false;
    if (bSwinging) return false;

    const float Now = GetWorld()->GetTimeSeconds();
    return (Now - LastSwingVisualTime) >= GetAttackInterval();
}

void AMeleeWeapon::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bSwinging || !SwingPivot || !GetWorld())
    {
        return;
    }

    const float t = GetWorld()->GetTimeSeconds() - SwingStartTime;

    if (t >= SwingDuration * 2.f)
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
        const float Ease = FMath::InterpEaseInOut(0.f, 1.f, rt, 2.0f);
        Alpha = 1.f - Ease;
    }

    const FRotator AddRot = FMath::Lerp(SwingRotA, SwingRotB, Alpha);
    SwingPivot->SetRelativeRotation(CachedPivotRot + AddRot);
}
