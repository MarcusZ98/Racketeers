// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CosmeticOption.h"
#include "PS_Base.h"
#include "GameModeStructs.h"
#include "PS_Lobby.generated.h"

class ALobbySpawnPoint;

USTRUCT(BlueprintType)
struct FLobbyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	ETeams Team;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	int TeamID;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	bool bIsReady;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	FCosmeticOption Cosmetic;

	FLobbyInfo()
	{
		PlayerName = "DEFAULT NAME";
		Team = ETeams::NONE;
		TeamID = -1;
		bIsReady = false;
		Cosmetic = FCosmeticOption();
	}
};



UCLASS()
class RACKETEERS_API APS_Lobby : public APS_Base
{
	GENERATED_BODY()

public:

	APS_Lobby();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FLobbyInfo LobbyInfo = FLobbyInfo();

	virtual void CopyProperties(APlayerState* PlayerState) override;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
