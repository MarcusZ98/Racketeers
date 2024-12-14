// Fill out your copyright notice in the Description page of Project Settings.


#include "Phase3GameMode.h"
#include "EngineUtils.h"

#include "AsyncTreeDifferences.h"
#include "Kismet/GameplayStatics.h"




/* void APhase3GameMode::GetActivePlayers()
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
        ProcessPlayerID(PlayerID, TeamID);
        SpawnBoat();
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
        // Set hull material
        ProcessPart(GameStateBase->PandasParts, MyPlayerID);
        break;

    case ETeams::TeamRaccoon:
        // Set hull material
        ProcessPart(GameStateBase->RaccoonParts, MyPlayerID);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid TeamID."));
        break;
        
    }
}

void APhase3GameMode::ProcessPlayerID(int32 MyPlayerID, ETeams Team)
{
    switch (Team)
    {
    case ETeams::TeamPanda:
        
        switch (MyPlayerID)
        {
        case 0:
            SpawnTag = "Panda0";
            break;
        case 1:
            SpawnTag = "Panda1";
            break;
        case 2:
            SpawnTag = "Panda2";
            break;
        default:
            break;
        }
        
    case ETeams::TeamRaccoon:
        switch (MyPlayerID)
        {
        case 0:
            SpawnTag = "Raccoon0";
            break;
        case 1:
            SpawnTag = "Raccoon1";
            break;
        case 2:
            SpawnTag = "Raccoon2";
            break;
        default:
            break;
        }

    case ETeams::NONE:
        break;
    }

}

void APhase3GameMode::ProcessPart(const FTeamShipParts& TeamParts,  int32 MyPlayerID)
{
    FBoatSettings BoatSettings;
    // Search through Cannnon Parts
    switch (static_cast<ECannonPart>(TeamParts.Cannon))
    {
    case ECannonPart::Cannon_0:
        BoatSettings.CannonAmount = (TeamID == ETeams::TeamPanda) ? 1 : 1;
        break;
    case ECannonPart::Cannon_1:
        BoatSettings.CannonAmount = (TeamID == ETeams::TeamPanda) ? 2 : 2;
        break;
    case ECannonPart::Cannon_2:
        BoatSettings.CannonAmount = (TeamID == ETeams::TeamPanda) ? 3 : 3;
        break;
    default:
        break;
    }

    // Search through Hull Parts
    switch (static_cast<EHullPart>(TeamParts.Hull))
    {
    case EHullPart::Hull_0:
        BoatSettings.Health = (TeamID == ETeams::TeamPanda) ? 90 : 90;
        BoatSettings.HullMaterial = (TeamID == ETeams::TeamPanda) ? PandaHullMaterial : RaccoonHullMaterial;
        SpawnActor = (TeamID == ETeams::TeamPanda) ? BoatSmall : BoatSmall;
        break;
    case EHullPart::Hull_1:
        BoatSettings.Health = (TeamID == ETeams::TeamPanda) ? 180 : 180;
        BoatSettings.HullMaterial = (TeamID == ETeams::TeamPanda) ? PandaHullMaterial : RaccoonHullMaterial;
        SpawnActor = (TeamID == ETeams::TeamPanda) ? BoatMedium : BoatMedium;
        break;
    case EHullPart::Hull_2:
        BoatSettings.Health = (TeamID == ETeams::TeamPanda) ? 360 : 360;
        BoatSettings.HullMaterial = (TeamID == ETeams::TeamPanda) ? PandaHullMaterial : RaccoonHullMaterial;
        SpawnActor = (TeamID == ETeams::TeamPanda) ? BoatBig : BoatBig;
        break;
    default:
        break;
    }

    // Search through Sail Parts
    switch (static_cast<ESailPart>(TeamParts.Sail))
    {
    case ESailPart::Sail_0:
        BoatSettings.MovementSpeed = (TeamID == ETeams::TeamPanda) ? 1150 : 1150;
        BoatSettings.RotationSpeed.Yaw = (TeamID == ETeams::TeamPanda) ? 500 : 500;
        BoatSettings.SailMaterial = (TeamID == ETeams::TeamPanda) ? PandaSailMaterial1 : RaccoonSailMaterial1;
        break;
    case ESailPart::Sail_1:
        BoatSettings.MovementSpeed = (TeamID == ETeams::TeamPanda) ? 1400 : 1400;
        BoatSettings.RotationSpeed.Yaw = (TeamID == ETeams::TeamPanda) ? 600 : 600;

        BoatSettings.SailMaterial = (TeamID == ETeams::TeamPanda) ? PandaSailMaterial2 : RaccoonSailMaterial2;
        break;
    case ESailPart::Sail_2:
        BoatSettings.MovementSpeed = (TeamID == ETeams::TeamPanda) ? 1750 : 1750;
        BoatSettings.RotationSpeed.Yaw = (TeamID == ETeams::TeamPanda) ? 700 : 700;
        BoatSettings.SailMaterial = (TeamID == ETeams::TeamPanda) ? PandaSailMaterial3 : RaccoonSailMaterial3;
        break;
    default:
        break;
    }
}

void APhase3GameMode::SpawnBoat()
{
    if (!SpawnActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnActor is not valid for SpawnTag: %s"), *SpawnTag.ToString());
        return;
    }

    // Find the actor with the specified SpawnTag
    AActor* SpawnPoint = nullptr;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        if (It->ActorHasTag(SpawnTag))
        {
            SpawnPoint = *It;
            break;
        }
    }

    if (!SpawnPoint)
    {
        UE_LOG(LogTemp, Warning, TEXT("No BoatSpawnStart actor found with SpawnTag: %s"), *SpawnTag.ToString());
        return;
    }

    // Get spawn location and rotation
    FVector SpawnLocation = SpawnPoint->GetActorLocation();
    FRotator SpawnRotation = SpawnPoint->GetActorRotation();

    // Pass the struct as spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    // Set custom initialization parameters via tags or directly in BoatSettings
    ABoatCharacter* SpawnedBoat = GetWorld()->SpawnActor<ABoatCharacter>(
        SpawnActor,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
        );
    
    if (!SpawnedBoat)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn boat for SpawnTag: %s"), *SpawnTag.ToString());
        return;
    }

    // Debug log
    UE_LOG(LogTemp, Log, TEXT("Boat successfully spawned at location: %s with tag: %s"), *SpawnLocation.ToString(), *SpawnTag.ToString());
}
*/
