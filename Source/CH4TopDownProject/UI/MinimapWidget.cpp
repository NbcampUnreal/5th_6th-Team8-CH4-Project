#include "UI/MinimapWidget.h"
#include "Components/Image.h"
#include "GameFramework/Pawn.h"
#include "EngineUtils.h"
#include "GameEvent/BlueZoneActor.h"

void UMinimapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    for (TActorIterator<ABlueZoneActor> It(GetWorld()); It; ++It)
    {
        BlueZoneActor = *It;
        break;
    }

    if (BlueZoneMaskImage)
    {
        BlueZoneMID = BlueZoneMaskImage->GetDynamicMaterial();
    }
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!BlueZoneActor || !BlueZoneMID)
    { 
        return; 
    }

    APawn* OwningPawn = GetOwningPlayerPawn();
    FVector CaptureCenter = OwningPawn ? OwningPawn->GetActorLocation() : FVector::ZeroVector;

    const FVector BlueZoneWorldPos = BlueZoneActor->GetActorLocation();

    float MapSize = 10000.0f;

    float U = (BlueZoneWorldPos.Y - CaptureCenter.Y) / MapSize + 0.5f;
    float V = (BlueZoneWorldPos.X - CaptureCenter.X) / MapSize + 0.5f;

    V = 1.0f - V;

    const FVector2D BlueZoneCenterUV(U, V);
    const float BlueZoneRadiusUV = BlueZoneActor->GetCurrentRadius() / MapSize;

    BlueZoneMID->SetVectorParameterValue(TEXT("BlueZoneCenterUV"), FVector(BlueZoneCenterUV, 0));
    BlueZoneMID->SetScalarParameterValue(TEXT("BlueZoneRadiusUV"), BlueZoneRadiusUV);
}