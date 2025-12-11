#pragma once 

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemData.generated.h"

// class ABaseItem; // 아이템
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Bag,
	Consumable,
	Equipment_Head,
	Equipment_Body,
	Ammo,
}; 


USTRUCT(BlueprintType) //구조체를 블루린트에서도 쓸 수 있게해주겠습니다
struct FItemData : public FTableRowBase // 구조체가 `데이터 테이블의 한줄`이 되도록 만드는 역할
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FName ItemID; // 아이템ID Apple, Potion

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FText ItemName; // 유저가 보는 아이템 이름 사과, 체력포션

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	UTexture2D* Thumbnail; // 인벤토리에서 보여줄 아이템 썸네일

	// 아이템을 만들고 주석을 풉니다.
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	// TSubclassOf<AActor> ItemActorClass; // 아이템의 설계도, 아이템 드롭에서 사용

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 Capacity; //인벤토리에서 차지할 공간

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 ContainerSize; //인벤토리에서 차지할 공간
	
};
