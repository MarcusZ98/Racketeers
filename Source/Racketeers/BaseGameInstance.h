// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "GameModeStructs.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */


enum ETransferState
{
	READY,
	EMPTY
};

USTRUCT(BlueprintType)
struct FGamePackage
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Tag = "EMPTY";
	UPROPERTY(BlueprintReadWrite)
	int32 ID = -1;
	 
};


USTRUCT(BlueprintType)
struct FGameStatsPackage{
	
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FResources RaccoonResources;
	UPROPERTY(BlueprintReadWrite)
	int32 RacconsRoundsWon = 0;
	UPROPERTY(BlueprintReadWrite)
	float RacconsBoatHealth = 0; 
	UPROPERTY(BlueprintReadWrite)
	TArray<TEnumAsByte<EPart>> RaccoonParts;
	
	UPROPERTY(BlueprintReadWrite)
	FResources PandaResources;
	UPROPERTY(BlueprintReadWrite)
	int32 RedPandasRoundsWon = 0;
	UPROPERTY(BlueprintReadWrite)
	float RedPandasBoatHealth = 0;
	UPROPERTY(BlueprintReadWrite)
	TArray<TEnumAsByte<EPart>> PandaParts;
	UPROPERTY(BlueprintReadWrite)
	int ExpectedPlayers;
	UPROPERTY(BlueprintReadWrite)
	ETeams WonTeam = ETeams::NONE;
	
	ETransferState State = EMPTY;
		
};

USTRUCT(BlueprintType)
struct FGameModeData 
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString LevelToLoad = "";
};



UCLASS()
class RACKETEERS_API UBaseGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()


public:
	
	UFUNCTION(Server, Reliable, WithValidation)
	void SetDataToTransfer(FGameStatsPackage GameStatsPackage);
	UFUNCTION(Server, Reliable, WithValidation)
	void SetGameStateData();
	UFUNCTION(Server, Reliable, WithValidation)
	void ClearDataStatsPackage();

	bool CheckIfDataToTransfer();
	
	FGameStatsPackage GetDataTransferPackage();
	
	UFUNCTION(BlueprintCallable)
	void SetGameModeData(FGameModeData Data);
	UFUNCTION(BlueprintCallable)
	FGameModeData GetGameModeData();

	//void ClearData(FRacketeersData* Data);
	


public:
	
private:
	UPROPERTY()
	FGameStatsPackage TempSavedGameStatsPackage;
	UPROPERTY()
	FGameModeData TempGameModeData;
	


	
	
	

	
	
};
