#include "UI/MinimapWidget.h"
#include "Components/Image.h"
#include "GameFramework/Pawn.h"

void UMinimapWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    //APawn* OwningPawn = GetOwningPlayerPawn();
    //if (OwningPawn && PlayerIcon)
    //{
    //    float PlayerYaw = OwningPawn->GetActorRotation().Yaw;
    //    PlayerIcon->SetRenderTransformAngle(PlayerYaw);
    //}
}
