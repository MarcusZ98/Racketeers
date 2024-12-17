// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.generated.h"

USTRUCT(BlueprintType)
struct FResources
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	int32 Wood;
	UPROPERTY(BlueprintReadWrite)
	int32 Fiber;
	UPROPERTY(BlueprintReadWrite)
	int32 Metal;

	FResources()
		: Wood(0), Fiber(0), Metal(0) {}
};

USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FResources Resources;
	UPROPERTY(BlueprintReadWrite)
	int32 RoundsWon;
	UPROPERTY(BlueprintReadWrite)
	float BoatHealth;
	UPROPERTY(BlueprintReadWrite)
	FString Name;

	FTeam()
	: Resources(), RoundsWon(0), BoatHealth(100.0f), Name(TEXT("Unnamed")) {}
};




UENUM(BlueprintType)
enum class ETeams : uint8
{
	TeamRaccoon = 0,
	TeamPanda = 1,
	NONE = 2,
};

UENUM(BlueprintType)
enum class EResources : uint8
{
	WOOD = 0,
	FIBER = 1,
	METAL = 2,
	NONE = 3
};

UENUM(BlueprintType, Blueprintable)
enum class ETransitionStates : uint8
{
	IN_PHASE,
	IN_TRANSITION_STATS,
};

USTRUCT(BlueprintType, Blueprintable)
struct FWidgetClassAndName
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget*  Widget = nullptr;
};
USTRUCT(BlueprintType)
struct FTimer
{
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Minutes = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Seconds = 0;
	
};

UENUM(Blueprintable)
enum EGameStats : uint8
{
	ALIVE = 0,
	PUSHES = 1,
	FALLS = 2,
	PARTCONSTRUCTED = 3,
	BOATDAMAGEDONE = 4,
	MISSES = 5
};

UENUM(BlueprintType)
enum class EHullPart: uint8
{
	Hull_0 = 0,
	Hull_1 = 1,
	Hull_2 = 2,
	Hull_NONE = 3,
};
UENUM(BlueprintType)
enum class ECannonPart : uint8
{
	Cannon_0 = 0,
	Cannon_1 = 1,
	Cannon_2 = 2,
	Cannon_NONE = 3,
};
UENUM(BlueprintType)
enum class ESailPart: uint8
{
	Sail_0 = 0,
	Sail_1 = 1,
	Sail_2 = 2,
	Sail_NONE = 3,
};

USTRUCT(BlueprintType)
struct FTeamGameStats
{
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite)
	int32 TeamAlive = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Pushes = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Falls = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PartConstructed = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BoatDamageDone = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Misses = 0;
};


USTRUCT(BlueprintType)
struct FGameStats
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTeamGameStats Raccoons;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTeamGameStats Pandas;
};

USTRUCT(BlueprintType)
struct FTeamShipParts
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EHullPart Hull;
	UPROPERTY(BlueprintReadWrite)
	ECannonPart Cannon;
	UPROPERTY(BlueprintReadWrite)
	ESailPart Sail;

	FTeamShipParts()
	{
		Hull = EHullPart::Hull_0;
		Cannon = ECannonPart::Cannon_0;
		Sail = ESailPart::Sail_0;
	}
};

UENUM(BlueprintType)
enum EPartSpacing
{
	HULL,
	CANNON,
	SAIL
};

USTRUCT (BlueprintType)
struct FCraftingProgress
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	int WoodAmount = 0;
	UPROPERTY(BlueprintReadWrite)
	int FiberAmount = 0;
	UPROPERTY(BlueprintReadWrite)
	int MetalAmount = 0;
};

USTRUCT(BlueprintType)
struct FTeamCraftingProgress
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FCraftingProgress HullProgress = {0, 0, 0};
	UPROPERTY(BlueprintReadWrite)
	FCraftingProgress CannonProgress = {0, 0, 0};
	UPROPERTY(BlueprintReadWrite)
	FCraftingProgress SailProgress = {0, 0, 0};
};