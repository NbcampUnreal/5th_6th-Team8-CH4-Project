#include "UI/DamageTextActor.h"
#include "Components/WidgetComponent.h"
#include "UI/DamageTextWidget.h"

ADamageTextActor::ADamageTextActor()
{ 	
	PrimaryActorTick.bCanEverTick = false;

	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComponent"));
	RootComponent = DamageWidgetComponent;

	DamageWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidgetComponent->SetDrawAtDesiredSize(true);
}

void ADamageTextActor::InitializeDamage(float Damage)
{
	UDamageTextWidget* DamageWidget = Cast<UDamageTextWidget>(DamageWidgetComponent->GetUserWidgetObject());
	if (DamageWidget)
	{
		DamageWidget->SetDamageText(Damage);
	}
}

void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeSpan);
}