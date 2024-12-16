// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "GameFramework/PlayerController.h"
#include "PC_Lobby.generated.h"

struct FCosmeticOption;
class ALobbySpawnPoint;
/**
 * 
 */
UCLASS()
class RACKETEERS_API APC_Lobby : public APlayerController
{
	GENERATED_BODY()

	// ----------------------------Variables--------------------------------------------
public:

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Game")
	ALobbySpawnPoint* SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* LobbyWidgetREF;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> TeamSelectionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> CosmeticWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* SpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* DespawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* ReadySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* UnreadySound;
	
	// ----------------------------Functions--------------------------------------------
public:
	
	UFUNCTION()
	virtual void BeginPlay() override;

	// Pass the start match event to the blueprint
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnStartMatch();

	// ----------------------------Server Functions--------------------------------------------

	// Spawn the player on server
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SpawnPlayer(APlayerController* PC, ETeams Team);

	// Remove the player on the server
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RemovePlayer();

	// Toggle the player's ready status
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ToggleReady(APlayerController* PC);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetCosmetic(APlayerController* PC, FCosmeticOption Cosmetic);
	

	// ----------------------------Client Functions--------------------------------------------

	// Show the lobby widget
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowLobbyWidget();

	// Show the team selection widget
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowTeamSelectionWidget();

	// Show the cosmetic widget
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowCosmeticWidget();

	// Start the match
	UFUNCTION(Client, Reliable)
	void Client_OnStartMatch();

	UFUNCTION(Client, Unreliable, BlueprintCallable)
	void Client_PlayToggleReadySound(bool bIsReady);

	// ----------------------------Multicast Functions--------------------------------------------

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlaySpawnSound();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayDespawnSound();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

