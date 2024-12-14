#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoatSettings.generated.h"
//#include "FBoatSettings.h"

USTRUCT(BlueprintType)
struct FBoatSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Settings")
	float Health;

	// Number of cannons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Settings")
	int32 CannonAmount;
	
	// Material for the sails
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Settings")
	UMaterialInterface* SailMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Boat Settings")
	UMaterialInterface* HullMaterial;

	// Movement speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Settings")
	float MovementSpeed;

	// Rotation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Settings")
	FRotator RotationSpeed;
	

	// Default constructor
	FBoatSettings()
		: Health(0)
		, CannonAmount(0)
		, SailMaterial(nullptr)
		, HullMaterial(nullptr)
		, MovementSpeed(0.0f)
		, RotationSpeed(0.0f)
	{
	}
};