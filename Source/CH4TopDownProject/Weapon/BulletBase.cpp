#include "Weapon/BulletBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "ObjectPool/ActorObjectPoolSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Character/RCPlayerCharacter.h"

ABulletBase::ABulletBase()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);
    NetDormancy = DORM_Awake;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(8.f);
    Collision->SetCollisionProfileName(TEXT("Projectile"));
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
    RootComponent = Collision;

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
    BulletMesh->SetupAttachment(RootComponent);
    BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->ProjectileGravityScale = 0.f;
    Movement->bAutoActivate = false;
    Movement->InitialSpeed = 0.f;
    Movement->MaxSpeed = 100000.f;
    Movement->bRotationFollowsVelocity = true;
    Movement->bShouldBounce = false;

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Collision->SetNotifyRigidBodyCollision(true);
}

void ABulletBase::BeginPlay()
{
    Super::BeginPlay();
}

void ABulletBase::InitBullet(
    const FVector& InDir,
    float InSpeed, 
    float InDamage,
    AController* InInstigatorController
)
{
    if (!HasAuthority())
        return;

    if (APawn* InstPawn = InInstigatorController ? InInstigatorController->GetPawn() : nullptr)
    {
        Collision->IgnoreActorWhenMoving(InstPawn, true);
        
        ARCPlayerCharacter* Player = Cast<ARCPlayerCharacter>(InstPawn);
        if (Player)
        {
            if (IsValid(Player->IgnoreActor))
            Collision->IgnoreActorWhenMoving(Player->IgnoreActor,true);
        }
    }

    if (AActor* OwnerActor = GetOwner())
    {
        Collision->IgnoreActorWhenMoving(OwnerActor, true);
    }

    Damage = InDamage;
    InstigatorController = InInstigatorController;

    const FVector Dir = InDir.GetSafeNormal();

    if (Movement)
    {
        Movement->StopMovementImmediately();
        Movement->Velocity = Dir * InSpeed;
        Movement->Activate(true);
    }

    StartLifeTimer();
}

void ABulletBase::StartLifeTimer()
{
    StopLifeTimer();

    if (HasAuthority() && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            LifeTimer,
            this,
            &ABulletBase::ReturnToPool,
            LifeTime,
            false
        );
        UE_LOG(LogTemp, Warning, TEXT("[Bullet][Server][LifeTimerStart] %s LifeTime=%.2f"),
            *GetName(), LifeTime);
    }
}

void ABulletBase::ReturnToPool_FromLifeTime()
{
    ReturnToPool_Internal(TEXT("LifeTimeExpired"));
}

void ABulletBase::ReturnToPool_Internal(const TCHAR* Reason)
{
    if (!HasAuthority())
        return;

    UE_LOG(LogTemp, Warning, TEXT("[Bullet][Server][ReturnToPool] %s Reason=%s"),
        *GetName(), Reason);

    if (UWorld* W = GetWorld())
    {
        if (UActorObjectPoolSubsystem* Pool = W->GetSubsystem<UActorObjectPoolSubsystem>())
        {
            Pool->ReturnToPool(this);
        }
    }
}

void ABulletBase::StopLifeTimer()
{
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(LifeTimer);
    }
}

void ABulletBase::ReturnToPool()
{
    ReturnToPool_Internal(TEXT("UnknownCaller"));
}

void ABulletBase::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, 
    const FHitResult& Hit
)
{
    if (!HasAuthority())
        return;

    APawn* InstPawn = InstigatorController.IsValid() ? InstigatorController->GetPawn() : nullptr;

    if (!OtherActor || OtherActor == InstPawn)
    {
        ReturnToPool_Internal(TEXT("HitInstigatorOrNull"));
        return;
    }

    const FVector ShotDir = (Movement ? Movement->Velocity.GetSafeNormal() : GetActorForwardVector());

    float FinalDamage = Damage;

    if (Hit.BoneName == HeadBoneName)
    {
		FinalDamage *= HeadshotMultiplier;
    }
    UGameplayStatics::ApplyPointDamage(
        OtherActor,
        FinalDamage,
        ShotDir,
        Hit,
        InstigatorController.Get(),
        this,
        nullptr
    );

    ReturnToPool_Internal(TEXT("HitAndDamaged"));
}

void ABulletBase::OnSpawnFromPool_Implementation()
{

    StopLifeTimer();

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);

    SetNetDormancy(DORM_Awake);
    FlushNetDormancy();

    Damage = 0.f;
    InstigatorController = nullptr;

    if (Movement)
    {
        Movement->StopMovementImmediately();
        Movement->Deactivate();
    }
}

void ABulletBase::OnReturnToPool_Implementation()
{
    StopLifeTimer();
    
    Collision->ClearMoveIgnoreActors();

    Damage = 0.f;
    InstigatorController = nullptr;

    if (Movement)
    {
        Movement->StopMovementImmediately();
        Movement->Deactivate();
    }

    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    SetNetDormancy(DORM_DormantAll);
}

UClass* ABulletBase::GetPoolKeyClass_Implementation()
{
    return GetClass();
}

void ABulletBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
