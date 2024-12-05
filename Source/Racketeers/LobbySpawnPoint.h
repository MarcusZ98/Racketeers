// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "LobbySpawnPoint.generated.h"


class APS_Lobby;
struct FLobbyInfo;
class UNiagaraSystem;

UCLASS()
class RACKETEERS_API ALobbySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbySpawnPoint();

	// ----------------------------Variables--------------------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* PlayerSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Replicated)
	UWidgetComponent* LobbyInfoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	TSubclassOf<AActor> PandaPlayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	TSubclassOf<AActor> RaccoonPlayerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team", Replicated)
	AActor* Player = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* SpawnVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	int TeamID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeams Team;

	// ----------------------------Functions--------------------------------------------
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnPlayer();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnVFX();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateWidgetInfo(const FLobbyInfo& NewLobbyInfo, APlayerState* PS);
	
	UFUNCTION(Server, Reliable)
	void Server_RemovePlayer();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleReady(bool bIsReady);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ToggleReadyStatus(bool bIsReady);

	void SetPlayerController(APlayerController* PC) { PlayerController = PC; }

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
