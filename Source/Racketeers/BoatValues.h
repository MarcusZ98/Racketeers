#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoatValues.generated.h"

USTRUCT(BlueprintType)
struct FBoatData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CannonAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* SailMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* HullMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Boat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnTag;

	
};

USTRUCT(BlueprintType)
struct FTeamParts
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FBoatData> PlayerValues;
};
