// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "GameFramework/PlayerController.h"
#include "PC_Lobby.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	ALobbySpawnPoint* SpawnPoint;

private:
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> TeamSelectionWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	UUserWidget* LobbyWidgetREF;

	
	// ----------------------------Functions--------------------------------------------
public:
	
	UFUNCTION()
	virtual void BeginPlay() override;
	
	// Show the team selection widget
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowTeamSelectionWidget();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowLobbyWidget();

	// Show the cosmetic widget
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowCosmeticWidget();
	
	// Spawn the player on server
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SpawnPlayer(APlayerController* PC, ETeams Team);

	// Remove the player on the server
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RemovePlayer();

	// Toggle the player's ready status
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ToggleReady();

	// Start the match
	UFUNCTION(Client, Reliable)
	void Client_OnStartMatch();

	// Pass the start match event to the blueprint
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnStartMatch();
};
