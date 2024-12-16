// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbySpawnPoint.h"
#include "WidgetLobbyInfo.h"
#include "NiagaraFunctionLibrary.h"
#include "PS_Lobby.h"
#include "RacketeersCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALobbySpawnPoint::ALobbySpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this actor to replicate
	bReplicates = true;

	// Create the Arrow Component
	PlayerSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	LobbyInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));

	// Attach the Arrow Component to the Root Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	PlayerSpawnPoint->SetupAttachment(RootComponent);
	LobbyInfoWidget->SetupAttachment(RootComponent);


	// Optional: Customize Arrow Appearance
	PlayerSpawnPoint->ArrowColor = FColor::Green;
	PlayerSpawnPoint->SetRelativeScale3D(FVector(1.0f));
}


// Spawn the player at the spawn point
void ALobbySpawnPoint::Server_SpawnPlayer_Implementation()
{
	if(HasAuthority())
	{
		// Set the player's spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn the player based on the team
		const TSubclassOf<AActor> PlayerClass = (Team == ETeams::TeamPanda) ? PandaPlayerClass : RaccoonPlayerClass;

		Player = GetWorld()->SpawnActor<AActor>(PlayerClass, PlayerSpawnPoint->GetComponentLocation(),PlayerSpawnPoint->GetComponentRotation(), SpawnParams);
		Player->SetActorEnableCollision(false);
		
		// Attach the player to the root component of this actor
		if (Player && GetRootComponent())
		{
			Player->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}

		Multicast_SpawnVFX();
	}
}

void ALobbySpawnPoint::Multicast_SpawnVFX_Implementation()
{
	// Spawn the VFX for player spawning
	if (SpawnVFX)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += 10.0f;
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnVFX, SpawnLocation, GetActorRotation());
		
		LobbyInfoWidget->SetVisibility(true);
	}
}


// Update the player's widget info for all clients
void ALobbySpawnPoint::Multicast_UpdateWidgetInfo_Implementation(const FLobbyInfo& NewLobbyInfo, APlayerState* PS)
{
	if (LobbyInfoWidget)
	{
		if (UWidgetLobbyInfo* LobbyInfo = Cast<UWidgetLobbyInfo>(LobbyInfoWidget->GetUserWidgetObject()))
		{
			LobbyInfo->UpdateLobbyInfo(NewLobbyInfo, PS);
		}
	}
}

// Remove the player from the spawn point
void ALobbySpawnPoint::Server_RemovePlayer_Implementation()
{
	if(Player && HasAuthority())
	{
		Player->Destroy();
		LobbyInfoWidget->SetVisibility(false);
		PlayerController = nullptr;
	}
}

void ALobbySpawnPoint::Multicast_ToggleReady_Implementation(bool bReady)
{
	if (UWidgetLobbyInfo* LobbyInfo = Cast<UWidgetLobbyInfo>(LobbyInfoWidget->GetUserWidgetObject()))
	{
		LobbyInfo->SetReadyStatus(bReady);
	}
}


void ALobbySpawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALobbySpawnPoint, Player);
	DOREPLIFETIME(ALobbySpawnPoint, LobbyInfoWidget);
}



