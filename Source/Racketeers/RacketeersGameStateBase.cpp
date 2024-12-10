// Fill out your copyright notice in the Description page of Project Settings.


#include "RacketeersGameStateBase.h"

#include <filesystem>

#include "BaseGameInstance.h"
#include "GameplayTagContainer.h"
#include "RacketeersGMBase.h"
#include "WidgetSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

class UWidgetSubsystem;

void ARacketeersGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARacketeersGameStateBase, RacconResource);
	DOREPLIFETIME(ARacketeersGameStateBase, RedPandasResource);

	DOREPLIFETIME(ARacketeersGameStateBase, TeamStats);
	DOREPLIFETIME(ARacketeersGameStateBase, RaccoonsGameStats);
	DOREPLIFETIME(ARacketeersGameStateBase, RedPandasGameStats);
	
	DOREPLIFETIME(ARacketeersGameStateBase, RacconsRoundsWon);
	DOREPLIFETIME(ARacketeersGameStateBase, RaccoonsBoatHealth);
	DOREPLIFETIME(ARacketeersGameStateBase, RaccoonsMaxHealth);
	
	DOREPLIFETIME(ARacketeersGameStateBase, RedPandasRoundsWon);
	DOREPLIFETIME(ARacketeersGameStateBase, RedPandasBoatHealth);
	DOREPLIFETIME(ARacketeersGameStateBase, RedPandasMaxHealth);

	DOREPLIFETIME(ARacketeersGameStateBase, GameWinner);
	DOREPLIFETIME(ARacketeersGameStateBase, CurrentPhase);
	DOREPLIFETIME(ARacketeersGameStateBase, IncomingPhase);
	
	DOREPLIFETIME(ARacketeersGameStateBase, Phase2RandomNumber);
	DOREPLIFETIME(ARacketeersGameStateBase, Phase3RandomNumber);
	
	DOREPLIFETIME(ARacketeersGameStateBase, RaccoonsReady);
	DOREPLIFETIME(ARacketeersGameStateBase, PandasReady);
}


ARacketeersGameStateBase::ARacketeersGameStateBase()
{
	AddPart(ETeams::Team_Raccoon, EPart::Cannon_0);
	AddPart(ETeams::Team_Raccoon, EPart::Hull_0);
	AddPart(ETeams::Team_Raccoon, EPart::Sail_0);
	AddPart(ETeams::Team_Panda, EPart::Cannon_0);
	AddPart(ETeams::Team_Panda, EPart::Hull_0);
	AddPart(ETeams::Team_Panda, EPart::Sail_0);
}

void ARacketeersGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		ARacketeersGMBase* GM = Cast<ARacketeersGMBase>( UGameplayStatics::GetGameMode(GetWorld()));
		if(GM)
		{
			CurrentPhase = GM->State;
		}
		UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetGameInstance());
		if (GI->CheckIfDataToTransfer())
		{
			FGameStatsPackage Package = GI->GetDataTransferPackage();
			RacconResource = Package.RaccoonResources;
			RacconsRoundsWon = Package.RacconsRoundsWon;
			RaccoonsBoatHealth = Package.RacconsBoatHealth;
			for (TEnumAsByte<EPart> RaccoonPart : Package.RaccoonParts)
			{
				RaccoonParts.Add(RaccoonPart);
			}
			RedPandasResource = Package.PandaResources;
			RedPandasRoundsWon = Package.RedPandasRoundsWon;
			RedPandasBoatHealth = Package.RedPandasBoatHealth;
			for (TEnumAsByte<EPart> PandaPart : Package.PandaParts)
			{
				PandaParts.Add(PandaPart);
			}
			GameWinner = Package.WonTeam;
			ExpectedPlayers = Package.ExpectedPlayers;
			GI->ClearDataStatsPackage();
		}

	}
	
	//UKismetSystemLibrary::K2_SetTimerDelegate()
	
	
	/*
	


	

	if (HasAuthority())
	{
		AddResource(3, EResources::WOOD, ETeams::Team_Raccoon);
		AddResource(3, EResources::WOOD, ETeams::Team_Panda);
		AddResource(3, EResources::FIBER, ETeams::Team_Raccoon);
		AddResource(3, EResources::FIBER, ETeams::Team_Panda);
		AddResource(3, EResources::METAL, ETeams::Team_Raccoon);
		AddResource(3, EResources::METAL, ETeams::Team_Panda);
	}
	RaccoonsBoatHealth = RaccoonsMaxHealth;
	RedPandasBoatHealth = RedPandasMaxHealth;

	*/
}

void ARacketeersGameStateBase::ChangeCurrentPhase(TEnumAsByte<EPhaseState> NewPhase)
{
	CurrentPhase = NewPhase;
	if(GetLocalRole() == ROLE_Authority)
	{
		OnRep_PhaseChange();
	}

}



int32 ARacketeersGameStateBase::GetTeamResources(ETeams Team, EResources Resource) const
{
	if(Team == ETeams::Team_Raccoon)
	{
		int Space = (int)Resource;
		int32* material = (int32*)((&RacconResource.Wood + Space));
		int32 MaterialAmount = material[0];
		return MaterialAmount;
	}
	int Space = (int)Resource;
	int32* material = (int32*)((&RedPandasResource.Wood + Space));
	int32 MaterialAmount = material[0];
	return MaterialAmount;
}

FTeamGameStats ARacketeersGameStateBase::GetTeamStats(ETeams Team)
{
	int TeamSpace = (int)Team;
	const FTeamGameStats* GameStats = (&TeamStats.Raccoons + TeamSpace); 
	return GameStats[0];
}

void ARacketeersGameStateBase::UpdateTeamAlive()
{
	TeamStats.Pandas.TeamAlive = 0;
	TeamStats.Raccoons.TeamAlive = 0;
	
	if(GetLocalRole() == ROLE_Authority)
	{
		for (APlayerState* PS : PlayerArray)
		{
			if (PS == nullptr) return; 
			APS_Base* PSBase = Cast<APS_Base>(PS);
			if (PSBase == nullptr) return;

			if(PSBase->PlayerInfo.Team == ETeams::Team_Raccoon)
			{
				AddToStats(1, EGameStats::ALIVE, ETeams::Team_Raccoon);
				continue;
			}
			if(PSBase->PlayerInfo.Team == ETeams::Team_Panda)
			{
				AddToStats(1, EGameStats::ALIVE, ETeams::Team_Panda);
				continue;
			}
		}
	}
}

void ARacketeersGameStateBase::UpdateHealth()
{
	APS_Base* PSBase = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerState<APS_Base>();
	if(PSBase == nullptr) return;
	PSBase->BoatHealth = PSBase->MaxBoatHealth;
}

bool ARacketeersGameStateBase::CheckTeamAlive(ETeams Team)
{
	if(GetTeamStats(Team).TeamAlive <= 0)
	{
		return false;
	}
	return true;
}

void ARacketeersGameStateBase::CheckRoundEnd(ETeams Team)
{
	if(!CheckTeamAlive(Team))
	{
		ARacketeersGMBase* GM = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
		GM->RoundCompletion();
	}
}

void ARacketeersGameStateBase::AddPart_Implementation(ETeams Team, EPart Part)
{
	if(Team == ETeams::Team_Raccoon)
	{
		RaccoonParts.Add(Part);
		return;
	}
	PandaParts.Add(Part);
}

void ARacketeersGameStateBase::RemovePart_Implementation(ETeams Team, EPart Part)
{
	if(Team == ETeams::Team_Raccoon)
	{
		RaccoonParts.Remove(Part);
	}
	PandaParts.Remove(Part);
}

void ARacketeersGameStateBase::SetMaxHealth_Implementation(ETeams Team, int32 MaxHealth)
{
	if(Team == ETeams::Team_Raccoon)
	{
		RaccoonsMaxHealth = MaxHealth;
		return;
	}
	RedPandasMaxHealth = MaxHealth;
}


void ARacketeersGameStateBase::DamageBoat(int Amount, ETeams Team)
{
	ARacketeersGMBase* GM = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM == nullptr || RaccoonsBoatHealth <= 0  ||RedPandasBoatHealth <= 0)
	{
		//UE_LOG(LogTemp, Error, TEXT("ARacketeersGameStateBase::DamageBoat GM is equal to nullptr"));
		return;
	}
	if(GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "IN STATE DAMAGE BOAT");
	}

	if (Team == ETeams::Team_Raccoon)
	{
		RaccoonsBoatHealth -= Amount;
		CheckOnRepHealthChanged();
		if (RaccoonsBoatHealth <= 0)
		{
			//call method in GameMode to set the victor and the score, either ending the game or go ti next phase based on what round the game is on
			//RedPandasRoundsWon++;
			GM->RoundCompletion();
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "GAME FINISHED");
		}
		return;
	}
	RedPandasBoatHealth -= Amount;
	CheckOnRepHealthChanged();
	if (RedPandasBoatHealth <= 0)
	{
		//call method in GameMode to set the victor and the score, either ending the game or go ti next phase based on what round the game is on
		//RacconsRoundsWon++;
		GM->RoundCompletion();
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "GAME FINISHED");
		
	}
}


void ARacketeersGameStateBase::RequestToRemoveWidget()
{
	ARacketeersGMBase* GM = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM == nullptr)
	{
		return;
	}
	UWidgetSubsystem* WS = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if (WS == nullptr)
	{
		return;
	}
	WS->IncrementPlayersPressed();

	//GM->UnloadWidget();
}


void ARacketeersGameStateBase::OnRep_PickUp()
{
	if(GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "PICKED UP");
	}
	GetGameInstance()->GetSubsystem<UWidgetSubsystem>()->OnPickUp.Broadcast();

}

void ARacketeersGameStateBase::OnRep_IncomingPhaseChange()
{
	switch (IncomingPhase)
	{
	case EPhaseState::Phase_1:
		OnIncomingPhaseOneActive.Broadcast();
		break;
	case EPhaseState::Phase_2:
		OnIncomingPhaseTwoActive.Broadcast();
		break;
	case EPhaseState::Phase_3:
		OnIncomingPhaseThreeActive.Broadcast();
		break;
	}
}

void ARacketeersGameStateBase::OnRep_PhaseChange()
{
	if(GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "PHASE CHANGE");
	}

	UWidgetSubsystem* WS = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if (WS == nullptr) return;
	UUserWidget* UserWidget = nullptr;
	switch (CurrentPhase)
	{
		case EPhaseState::Phase_1:
			OnPhaseOneActive.Broadcast();
			WS->ActivateWidget("TeamResources");
			WS->RemoveWidget("TeamHealth");
		break;
		case EPhaseState::Phase_2:
			OnPhaseTwoActive.Broadcast();
			break;
		case EPhaseState::Phase_3:
			UpdateHealth();
			UpdateTeamAlive();
			OnPhaseThreeActive.Broadcast();
			//WS->ActivateWidget("TeamHealth");
			WS->RemoveWidget("TeamResources");
			break;
	}
	//UpdateTeamAlive();
}

void ARacketeersGameStateBase::SetRandomNumber(int Number)
{
	Phase2RandomNumber = Number;
}

void ARacketeersGameStateBase::AddResource_Implementation(int Amount, EResources Resource, ETeams Team)
{
	if (Team == ETeams::Team_Raccoon)
	{
		int Space = (int)Resource;
		int32* material = (int32*)((&RacconResource.Wood + Space));
		if (material == nullptr)
		{
			return;
		}
		material[0] += Amount;
		
		if(UGameplayStatics::GetGameState(GetWorld())->GetLocalRole()  == ENetRole::ROLE_Authority)
		{
			OnRep_PickUp();
		}

		return;
	}
	int Space = (int)Resource;
	int32* material = (int32*)((&RedPandasResource.Wood + Space));
	if (material == nullptr)
	{
		return;
	}
	material[0] += Amount;


	if(UGameplayStatics::GetGameState(GetWorld())->GetLocalRole()  == ENetRole::ROLE_Authority)
	{
		OnRep_PickUp();
	}

	
}

void ARacketeersGameStateBase::AddToStats_Implementation(int Amount, EGameStats Stat, ETeams Team)
{
	int TeamSpace = (int)Team * sizeof(EGameStats);
	int StatSpace = (int)Stat;
	
	FTeamGameStats* TeamGameStats = ((&TeamStats.Raccoons + TeamSpace));
	int32* Stats = ((&TeamGameStats->TeamAlive + StatSpace));
	if(Stats == nullptr) return; 
	Stats[0] += Amount;
}

//Callas på clienten sen på servern
void ARacketeersGameStateBase::RemoveResource_Implementation(int Amount, EResources Resource, ETeams Team)
{
	if (Team == ETeams::Team_Raccoon)
	{
		int Space = (int)Resource;
		int32* material = (int32*)((&RacconResource.Wood + Space));
		if (material == nullptr)
		{
			return;
		}
		material[0] -= Amount;
		if (material[0] < 0)
		{
			material[0] = 0;
		}
		if(this->GetLocalRole() == ENetRole::ROLE_Authority)
		{
			OnRep_PickUp();
		}
		return;
	}

	int Space = (int)Resource;
	int32* material = (int32*)((&RedPandasResource.Wood + Space));
	if (material == nullptr)
	{
		return;
	}
	material[0] -= Amount;
	if (material[0] < 0)
	{
		material[0] = 0;
	}
	if(this->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		OnRep_PickUp();
	}

}

void ARacketeersGameStateBase::OnRep_HealthChanged()
{
	if(GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "HEALTH CHANGED");
	}
	GetGameInstance()->GetSubsystem<UWidgetSubsystem>()->OnDamaged.Broadcast();
}

inline void ARacketeersGameStateBase::CheckOnRepHealthChanged()
{
	if(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetLocalRole()  == ENetRole::ROLE_Authority)
	{
		OnRep_HealthChanged();
	}
}