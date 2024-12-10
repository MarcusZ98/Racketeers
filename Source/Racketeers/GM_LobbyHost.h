// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GM_Base.h"
#include "LobbySpawnPoint.h"
#include "GM_LobbyHost.generated.h"

/**
 * 
 */
UCLASS()
class RACKETEERS_API AGM_LobbyHost : public AGM_Base
{
	GENERATED_BODY()
	
	// ----------------- Variables -----------------

	UPROPERTY(EditAnywhere, Category = "Players")
	TArray<AActor*> SpawnPositions;

	UPROPERTY(EditAnywhere, Category = "Players")
	TArray<APlayerController*> Players;

	UPROPERTY(EditAnywhere, Category = "Game Rules")
	bool bAllowUnbalancedTeams;
	
	UPROPERTY(EditAnywhere, Category = "Game Rules")
	FString MapName = "World_P";

	// ----------------- Functions -----------------

public:
	
	UFUNCTION()
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnPostLogin(AController* NewPlayer) override;

	UFUNCTION()
	void SpawnPlayer(APlayerController* PC, ETeams Team);

	UFUNCTION(BlueprintCallable)
	void RemovePlayer(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayers();
	
	UFUNCTION(BlueprintCallable)
	void StartTheMatch();

private:

	void SetUpSpawnPositions();
	void UpdateIfTeamFull();
	void UpdateIfEnoughPlayersToStart() const;
	void SetPlayerIDs();
	
};
