#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class UImage;

UCLASS()
class CH4TOPDOWNPROJECT_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> MinimapImage;

    //UPROPERTY(meta = (BindWidget))
    //TObjectPtr<UImage> PlayerIcon;
};
