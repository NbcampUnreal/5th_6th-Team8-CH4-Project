#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmorBase.generated.h"

UCLASS()
class CH4TOPDOWNPROJECT_API AArmorBase : public AActor
{
    GENERATED_BODY()

public:
    AArmorBase();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    float ModifyDamage(float InDamage) const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Armor")
    TObjectPtr<UStaticMeshComponent> ArmorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor", meta = (ClampMin = "1", ClampMax = "3"))
    int32 ArmorLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor")
    float DefensePerLevel = 0.1f;
};
