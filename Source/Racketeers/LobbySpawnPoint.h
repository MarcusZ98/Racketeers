// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "LobbySpawnPoint.generated.h"


class APS_Lobby;
class UNiagaraSystem;

UCLASS()
class RACKETEERS_API ALobbySpawnPoint : public AActor
{
	GENERATED_BODY()
	
	// ----------------------------Variables--------------------------------------------

public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* PlayerSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Replicated)
	UWidgetComponent* LobbyInfoWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int TeamID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	ETeams Team;

private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Classes", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> RaccoonPlayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Classes", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> PandaPlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* SpawnVFX;
	
	UPROPERTY(EditAnywhere, Category = "Player")
	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Player", Replicated)
	AActor* Player = nullptr;
	
	

	// ----------------------------Functions--------------------------------------------

public:

	ALobbySpawnPoint();

	// Spawn the player on the spawn point
	UFUNCTION(Server, Reliable)
	void Server_SpawnPlayer();

	//Update the widget info on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateWidgetInfo(const FLobbyInfo& NewLobbyInfo, APlayerState* PS);

	// Toggle the ready status on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleReady(bool bIsReady);

	// Remove the player from the spawn point
	UFUNCTION(Server, Reliable)
	void Server_RemovePlayer();

	// Get/Set Player Controller
	void SetPlayerController(APlayerController* PC) { PlayerController = PC; }
	APlayerController* GetPlayerController() const { return PlayerController; }

	bool IsOccupied() const { return PlayerController != nullptr; }

private:

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnVFX();


protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
