
#include "Interface/Barricade.h"
#include <Kismet/GameplayStatics.h>
#include <Character/RCPlayerCharacter.h>

ABarricade::ABarricade()
{

}

void ABarricade::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ARCPlayerCharacter>())
	{
		this->SetOwner(OtherActor);

		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString::Printf(
				TEXT("[WorldItemBase] Begin Overlap : %s"),
				*OtherActor->GetName()
			)
		);
	}
	
}

void ABarricade::TakeDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (!bCanTakeDamage)
	{
		return;
	}

	CurrentHP -= Damage;

	if (CurrentHP <= 0)
	{
		if (HasAuthority())
		{
			HandleDestroyed();
		}
		else
		{
			Multicast_OnDeath();
		}
	}
}

void ABarricade::Multicast_OnDeath_Implementation()
{
	if (IsNetMode(NM_DedicatedServer))
	{
		return; // 데디 서버에서는 실행 안 함
	}

	if (DeathParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DeathParticle,
			GetActorTransform()
		);
	}
}
