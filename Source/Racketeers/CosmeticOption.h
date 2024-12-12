#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "CosmeticOption.generated.h"

/**
 * Enum for cosmetic types
 */
UENUM(BlueprintType)
enum class ECosmeticType : uint8
{
	HAT         UMETA(DisplayName = "Hat"),
	CLOTHING    UMETA(DisplayName = "Clothing"),
	ACCESSORY   UMETA(DisplayName = "Accessory"),
	DEFAULT     UMETA(DisplayName = "Default"),
};

/**
 * Struct for cosmetic options
 */
USTRUCT(BlueprintType)
struct FCosmeticOption : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetics")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetics")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetics")
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetics")
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetics")
	ECosmeticType Type;

	FCosmeticOption()
		: Name(TEXT("Default"))
		, Icon(nullptr)
		, Actor(nullptr)
		, Material(nullptr)
		, Type(ECosmeticType::DEFAULT)
	{}
};
