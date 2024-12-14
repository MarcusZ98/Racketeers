// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Lobby.h"
#include "GM_LobbyHost.h"
#include "PS_Lobby.h"
#include "RacketeersCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void APC_Lobby::BeginPlay()
{
	Super::BeginPlay();
}


// Show the team selection widget
void APC_Lobby::Client_ShowTeamSelectionWidget_Implementation()
{
	// Create the team selection widget
	if (TeamSelectionWidgetClass)
	{
		if (UUserWidget* TeamSelectionWidget = CreateWidget<UUserWidget>(this, TeamSelectionWidgetClass))
		{
			TeamSelectionWidget->AddToViewport();
			bShowMouseCursor = true; // Show the cursor for UI interaction
			SetInputMode(FInputModeGameAndUI());
		}
	}
}

void APC_Lobby::Client_ShowLobbyWidget_Implementation()
{
	if (LobbyWidgetClass)
	{
		if (UUserWidget* LobbyWidget = CreateWidget<UUserWidget>(this, LobbyWidgetClass))
		{
			LobbyWidget->AddToViewport();
			LobbyWidgetREF = LobbyWidget;
		}
	}
}

void APC_Lobby::Client_ShowCosmeticWidget_Implementation()
{
	if (CosmeticWidgetClass)
	{
		if (UUserWidget* CosmeticWidget = CreateWidget<UUserWidget>(this, CosmeticWidgetClass))
		{
			CosmeticWidget->AddToViewport();
		}
	}
}

void APC_Lobby::Server_SpawnPlayer_Implementation(APlayerController* PC, ETeams Team)
{
	
	if (AGM_LobbyHost* GameMode = Cast<AGM_LobbyHost>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->SpawnPlayer(PC, Team);
		Multicast_PlaySpawnSound();
	}
	Cast<APC_Lobby>(PC)->Client_ShowLobbyWidget();
}

void APC_Lobby::Server_RemovePlayer_Implementation()
{
	if (AGM_LobbyHost* GameMode = Cast<AGM_LobbyHost>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->RemovePlayer(this);
		Multicast_PlayDespawnSound();
	}
}

void APC_Lobby::Server_ToggleReady_Implementation(APlayerController* PC)
{
	if (APS_Lobby* PS = Cast<APS_Lobby>(PlayerState))
	{
		PS->LobbyInfo.bIsReady = !PS->LobbyInfo.bIsReady;
		Cast<APC_Lobby>(PC)->Client_PlayToggleReadySound(PS->LobbyInfo.bIsReady);
	}

	if (AGM_LobbyHost* GameMode = Cast<AGM_LobbyHost>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->UpdatePlayers();
	}
}

void APC_Lobby::Server_SetCosmetic_Implementation(APlayerController* PC, FCosmeticOption Cosmetic)
{
	if (ARacketeersCharacter* Pl = Cast<ARacketeersCharacter>(Cast<APC_Lobby>(PC)->SpawnPoint->Player))
	{
		Cast<APS_Lobby>(PlayerState)->LobbyInfo.Cosmetic = Cosmetic;
		Pl->Cosmetic = Cosmetic;
		Pl->OnRep_Cosmetic();
	}
}

void APC_Lobby::Client_OnStartMatch_Implementation()
{
	OnStartMatch();
}

void APC_Lobby::Multicast_PlaySpawnSound_Implementation()
{
	if (SpawnSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SpawnSound);
	}
}

void APC_Lobby::Multicast_PlayDespawnSound_Implementation()
{
	if (DespawnSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DespawnSound);
	}
}

void APC_Lobby::Client_PlayToggleReadySound_Implementation(bool bIsReady)
{
	if (bIsReady)
	{
		if (ReadySound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ReadySound);
		}
	}
	else
	{
		if (UnreadySound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), UnreadySound);
		}
	}
}

void APC_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APC_Lobby, SpawnPoint);
}
