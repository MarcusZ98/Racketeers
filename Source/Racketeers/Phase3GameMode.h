// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoatSettings.h"
#include "BoatCharacter.h"
#include "RacketeersGameStateBase.h"
#include "GameModeStructs.h"
#include "RacketeersGMBase.h"
#include "Phase3GameMode.generated.h"

/**
 * 
 */
UCLASS()
class RACKETEERS_API APhase3GameMode : public ARacketeersGMBase
{
	GENERATED_BODY()

/* public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void GetActivePlayers();

protected:
	//void SpawnBoat();
	void SetBoatValues(ETeams MyTeamID, int32 MyPlayerID);
	void ProcessTeamParts(const FTeamShipParts& TeamParts, int32 MyPlayerID);
	void ProcessPlayerID(int32 MyPlayerID, ETeams Team);
	void ProcessPart(const FTeamShipParts& TeamParts,  int32 MyPlayerID);
	void SpawnBoat();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team Data")
	TMap<ETeams, FTeamShipParts> TeamData;
	
	//UPROPERTY()
	//APlayerState* PlayerState;
	UPROPERTY()
	APS_Base* CustomPlayerState;
	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	ABoatCharacter* BoatCharacter;
	UPROPERTY()
	UMaterialInterface* Material;
	UPROPERTY()
	ARacketeersGameStateBase* GameStateBase;
	UPROPERTY()
	UClass* SpawnActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boats")
	UClass* BoatSmall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boats")
	UClass* BoatMedium;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boats")
	UClass* BoatBig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* RaccoonHullMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* PandaHullMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* RaccoonSailMaterial1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* RaccoonSailMaterial2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* RaccoonSailMaterial3;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* PandaSailMaterial1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* PandaSailMaterial2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat Materials")
	UMaterialInterface* PandaSailMaterial3;
	
	float PlayerIndex;
	FPlayerInfo PlayerInfo;
	ETeams TeamID;
	int32 PlayerID;
	FName SpawnTag;
	
private:

	
*/
};
