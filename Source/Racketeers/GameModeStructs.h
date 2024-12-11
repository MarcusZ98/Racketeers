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
	Team_Raccoon,
	Team_Panda,
	NONE,
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

UENUM()
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
enum EPart
{
	Hull_0,
	Hull_1,
	Hull_2,
	Cannon_0,
	Cannon_1,
	Cannon_2,
	Sail_0,
	Sail_1,
	Sail_2,
	NONE,
};

USTRUCT(BlueprintType)
struct FParts
{
	GENERATED_BODY()
	EPart Hull = Hull_0;
	EPart Cannon = Cannon_0;
	EPart Sail = Sail_0;
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
	EPart Hull = Hull_0;
	EPart Cannon = Cannon_0;
	EPart Sail = Sail_0;
};


UENUM(BlueprintType)
enum EPartSpacing
{
	HULL,
	CANNON,
	SAIL
};
