#include "Armor/ArmorBase.h"
#include "Net/UnrealNetwork.h"

AArmorBase::AArmorBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    ArmorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmorMesh"));
    SetRootComponent(ArmorMesh);

    ArmorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ArmorMesh->SetIsReplicated(true);
}

void AArmorBase::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

float AArmorBase::ModifyDamage(float InDamage) const
{
    const float DefenseRate = FMath::Clamp(
        ArmorLevel * DefensePerLevel,
        0.f,
        0.3f
    );

    const float ReducedDamage = InDamage * (1.f - DefenseRate);

    UE_LOG(LogTemp, Log,
        TEXT("[Armor] Level=%d DefenseRate=%.0f%% Damage %.1f -> %.1f"),
        ArmorLevel,
        DefenseRate * 100.f,
        InDamage,
        ReducedDamage
    );

    return ReducedDamage;
}