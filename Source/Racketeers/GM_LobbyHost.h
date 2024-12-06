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

public:
	
	// ----------------- Variables -----------------

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> SpawnPositions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowUnbalancedTeams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	FString MapName = "World_P";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	TArray<APlayerController*> Players;

	// ----------------- Functions -----------------
	
	UFUNCTION()
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnPostLogin(AController* NewPlayer) override;
	
	UFUNCTION()
	void SetUpSpawnPositions();

	UFUNCTION()
	void SpawnPlayer(APlayerController* PC, ETeams Team);

	UFUNCTION(BlueprintCallable)
	void RemovePlayer(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayers();

	UFUNCTION()
	void UpdateIfTeamFull();
	
	UFUNCTION()
	void UpdateIfEnoughPlayersToStart() const;

	UFUNCTION(BlueprintCallable)
	void StartTheMatch();

	
};
