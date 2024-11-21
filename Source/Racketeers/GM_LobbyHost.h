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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> PandaPositions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> RaccoonPositions;

	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPostLogin(AController* NewPlayer) override;

	UFUNCTION()
	virtual void OnLogout(AController* Exiting);
	
	UFUNCTION()
	void SetUpSpawnPositions();

	UFUNCTION()
	void SpawnPlayer(APlayerController* PC, ETeams Team);

	UFUNCTION()
	void RemovePlayer(APlayerController* PC);
};