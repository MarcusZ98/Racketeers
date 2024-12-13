// Fill out your copyright notice in the Description page of Project Settings.


#include "Phase3GameMode.h"

#include "Kismet/GameplayStatics.h"

void APhase3GameMode::BeginPlay()
{
    Super::BeginPlay();

    GetActivePlayers();
    //SetBoatValues(team)
}


void APhase3GameMode::GetActivePlayers()
{
    GameState = GetWorld()->GetGameState<AGameStateBase>();
    if (!GameState) return;

    for (APlayerState* PlayerState : GameState->PlayerArray)
    {
        if (!PlayerState) continue;

        CustomPlayerState = Cast<APS_Base>(PlayerState);
        if (!CustomPlayerState) continue;

        TeamID = CustomPlayerState->PlayerInfo.Team;
        PlayerID = CustomPlayerState->PlayerInfo.TeamPlayerID;

        SetBoatValues(TeamID, PlayerID);
    }
}

void APhase3GameMode::SetBoatValues(ETeams MyTeamID, int32 MyPlayerID)
{
    // Validate BoatCharacter
    BoatCharacter = Cast<ABoatCharacter>(UGameplayStatics::GetPlayerPawn(this, PlayerID - 1));
    if (!BoatCharacter) return;

    // Handle teams
    switch (MyTeamID)
    {
    case ETeams::TeamPanda:
        ProcessTeamParts(GameStateBase->PandasParts, MyPlayerID, "Panda");
        break;

    case ETeams::TeamRaccoon:
        ProcessTeamParts(GameStateBase->RaccoonParts, MyPlayerID, "Raccoon");
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid TeamID."));
        break;
    }
}

void APhase3GameMode::ProcessTeamParts(const FTeamShipParts& TeamParts, int32 MyPlayerID, const FString& TeamName)
{
    // Handle PlayerID logic
    ProcessPlayerID(MyPlayerID, TeamName);

    // Handle individual parts
    switch (TeamParts)
    {
    case CANNON:
        ProcessPart(1, TEXT("Cannon"));
        break;
    case HULL:
        ProcessPart(2 , TEXT("Hull"));
        break;
    case SAIL:
        ProcessPart(3 , TEXT("Sail"));
        break;
    }
}

void APhase3GameMode::ProcessPlayerID(int32 MyPlayerID, const FString& TeamName)
{
    switch (MyPlayerID)
    {
    case 0:
        UE_LOG(LogTemp, Log, TEXT("%s Player 0 logic."), *TeamName);
        // Player 0-specific logic
        break;
    case 1:
        UE_LOG(LogTemp, Log, TEXT("%s Player 1 logic."), *TeamName);
        // Player 1-specific logic
        break;
    case 2:
        UE_LOG(LogTemp, Log, TEXT("%s Player 2 logic."), *TeamName);
        // Player 2-specific logic
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerID for %s team."), *TeamName);
        break;
    }
}

void APhase3GameMode::ProcessPart(int32 PartValue, const FString& PartName)
{
    // Handle part based on its integer value
    switch (PartValue)
    {
    case 0:
        UE_LOG(LogTemp, Log, TEXT("%s value is 0. Applying logic."), *PartName);
        // Logic for value 0
        break;
    case 1:
        UE_LOG(LogTemp, Log, TEXT("%s value is 1. Applying logic."), *PartName);
        // Logic for value 1
        break;
    case 2:
        UE_LOG(LogTemp, Log, TEXT("%s value is 2. Applying logic."), *PartName);
        // Logic for value 2
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid value for %s: %d."), *PartName, PartValue);
        break;
    }
}

