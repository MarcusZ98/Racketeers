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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* LobbyWidgetREF;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> TeamSelectionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> CosmeticWidgetClass;



	
	// ----------------------------Functions--------------------------------------------
public:
	
	UFUNCTION()
	virtual void BeginPlay() override;
	
	// Show the team selection widget
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_ShowTeamSelectionWidget();

	// Show the lobby widget
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
