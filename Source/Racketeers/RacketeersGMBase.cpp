// Fill out your copyright notice in the Description page of Project Settings.


#include "RacketeersGMBase.h"

#include <steam/steamclientpublic.h>

#include "BaseGameInstance.h"
#include "EngineUtils.h"
#include "BoatCharacter.h"
#include "FrameTypes.h"
#include "HeadMountedDisplayTypes.h"
#include "PS_Base.h"
#include "RacketeersController.h"
#include "RacketeersGameStateBase.h"
#include "TransitionComponent.h"
#include "WidgetSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


//Göra ett event som en klass får som senare aktiverar widgeten.




/*
 *	LoadIntoPhaseOne - (PlayerStart)
 *	Phase 1 completed
 *	Move Players To Phase2 level
 *
 *
 *	Servern (GameMode) Delar in De olika spelarna
	 *	SpawnPointSystem - Decides Where players Should Spawn
	 *	TeamSpawnPoints - The SpawnPoints To use
	 *	SpawnPoints - Get Local Player Move To The Location
 *	[GameMode] - Server Only
 *		
 *
 */

class UWidgetSubsystem;
class APS_Base;


ARacketeersGMBase::ARacketeersGMBase()
{
	UE_LOG(LogTemp, Warning, TEXT("AGM_Base::AGM_Base"));
	TransitionTimer = 5;
}

void ARacketeersGMBase::PlayerControllerConstructed()
{
}

void ARacketeersGMBase::OnPostLogin(AController* NewPlayer)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,15,FColor::Yellow, "OnPostLogin");
	}
	const APlayerState* NewPlayerState = NewPlayer->PlayerState;
	if(NewPlayerState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1,15,FColor::Yellow, "PLayer State NULLPTR");
		return;
	}
	for (APlayerState* JoiningState : JoiningPlayerStates)
	{
		GEngine->AddOnScreenDebugMessage(-1,15,FColor::Yellow, "Inactiveplayer: " + JoiningState->SavedNetworkAddress + " NewPlayer: " + NewPlayerState->SavedNetworkAddress);
		if(JoiningState->SavedNetworkAddress == NewPlayerState->SavedNetworkAddress)
		{
		
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,15,FColor::Yellow, "ALLOWED TO REJOIN");
			}
			Super::OnPostLogin(NewPlayer);
			APlayerController* PC = NewPlayerState->GetPlayerController();
			JoiningPlayersControllers.Add(PC);
			JoiningPlayerStates.Remove(JoiningState);

			if(PC && NewPlayerState->IsSpectator()) 
			{
				Cast<ARacketeersController>(PC)->SetPlayerSpectator();
			}
			return;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1,15,FColor::Yellow, "Destroy PLayer COntroller");
	NewPlayer->Destroy();

}

void ARacketeersGMBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ARacketeersGMBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                                 FString& ErrorMessage)
{
	for (APlayerState* PlayerState : InactivePlayerArray)
	{
		if(PlayerState->SavedNetworkAddress == Address)
		{
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,15,FColor::Blue, Address);
			}
			JoiningPlayerStates.Add(PlayerState);
		}
	}
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* ARacketeersGMBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void ARacketeersGMBase::Logout(AController* Exiting)
{
	if(Exiting)
	{
		InactivePlayerArray.Add(Exiting->PlayerState);
	}
	Super::Logout(Exiting);
}

void ARacketeersGMBase::AddInactivePlayer(APlayerState* PlayerState, APlayerController* PC)
{
	APS_Base* PS = Cast<APS_Base>(PlayerState);
	if(PS)
	{
		PS->PlayerNetworkData.bIsReconnecting = true;
	}
	
	Super::AddInactivePlayer(PlayerState, PC);
}

void ARacketeersGMBase::FreezePlayers()
{
	AGameState* GS = GetGameState<AGameState>();
	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		APlayerController* PC = PlayerState->GetPlayerController();
		UCharacterMovementComponent* CMC = PC->GetCharacter()->GetCharacterMovement();
		if(PC && CMC)
		{

			CMC->SetMovementMode(MOVE_None);
			/*
			PC->DisableInput(PC);
			PC->ChangeState(NAME_Inactive);
			PC->ClientGotoState(NAME_Inactive);
			*/
		}
	}
}

void ARacketeersGMBase::UnFreezePlayers()
{
	AGameState* GS = GetGameState<AGameState>();
	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		APlayerController* PC = PlayerState->GetPlayerController();
		UCharacterMovementComponent* CMC = PC->GetCharacter()->GetCharacterMovement();
		if(PC && CMC)
		{

			CMC->SetMovementMode(MOVE_Walking);
			/*
			PC->DisableInput(PC);
			PC->ChangeState(NAME_Playing);
			PC->ClientGotoState(NAME_Playing);
			*/
		}
	}
}

AActor* ARacketeersGMBase::ChoosePlayerStart_Implementation(AController* Player)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "ARacketeersGMBase::ChoosePlayerStart_Implementation");
	return Super::ChoosePlayerStart_Implementation(Player);
}

AActor* ARacketeersGMBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "ARacketeersGMBase::FindPlayerStart_Implementation");
	UWorld* World = GetWorld();
	// If incoming start is specified, then just use it
	if (!IncomingName.IsEmpty())
	{
		const FName IncomingPlayerStartTag = FName(*IncomingName);
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start && Start->PlayerStartTag == IncomingPlayerStartTag)
			{
				return Start;
			}
		}
	}

	// Always pick StartSpot at start of match
	if (ShouldSpawnAtStartSpot(Player))
	{
		if (AActor* PlayerStartSpot = Player->StartSpot.Get())
		{
			return PlayerStartSpot;
		}
		else
		{
			UE_LOG(LogGameMode, Error, TEXT("FindPlayerStart: ShouldSpawnAtStartSpot returned true but the Player StartSpot was null."));
		}
	}


	AActor* BestStart = ChoosePlayerStart(Player);
	if (BestStart == nullptr)
	{
		// No player start found
		UE_LOG(LogGameMode, Log, TEXT("FindPlayerStart: PATHS NOT DEFINED or NO PLAYERSTART with positive rating"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "FindPlayerStart: PATHS NOT DEFINED or NO PLAYERSTART with positive rating");
		// This is a bit odd, but there was a complex chunk of code that in the end always resulted in this, so we may as well just 
		// short cut it down to this.  Basically we are saying spawn at 0,0,0 if we didn't find a proper player start
		BestStart = World->GetWorldSettings();
	}

	return BestStart;
	
}

void ARacketeersGMBase::UnloadWidget()
{
	UnloadWidgetCount++;
	if(UnloadWidgetCount == NumPlayers)
	{
			bIsGameActive = true;
			UnloadWidgetCount=0;
	}
}

void ARacketeersGMBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogTemp, Warning, TEXT("AGM_Base::Initiate Game"));
	
}


void ARacketeersGMBase::BeginPlay()
{

	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("RacketeersGMBase::BeginPlay"));
	
	//Set the GameState in GameMode
	//GameState = Cast<AGS_Base>(UGameplayStatics::GetGameState(GetWorld()));


	/*
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetGameInstance());
	if(GI)
	{
		FGameModeData Data = GI->GetGameModeData();
		LevelToLoad = Data.LevelToLoad;
		
	}
	*/
	
	bUseSeamlessTravel = true;
	WidgetSubsystem = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();

	//Initilize variables
	Phase_1 = NewObject<UPhase>();
	Phase_2 = NewObject<UPhase>();
	Phase_3 = NewObject<UPhase>();
	FActorSpawnParameters ASPParams;

	FVector* V = new FVector(0,0,0);
	FRotator* RO = new FRotator(0,0,0);
	
	//UE_LOG(LogTemp, Warning, TEXT("About To Spawn Timer Info"));
	TimerInfo = Cast<ATimerInfo>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimerInfo::StaticClass()));

	
	if(TimerInfo == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RacketeersGMBase::SpawnActor is nullptr"));
	}
	
	LevelLoadingManager = Cast<ALevelLoadingManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelLoadingManager::StaticClass()));

	if(LevelLoadingManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: LevelLoadingManager Return nullptr, ARacketeersGMBase::BeginPlay"));
	}

	
	//Declare the variables 
	Phase_1->State = EPhaseState::Phase_1;
	Phase_1->TimeLimit = 90.0f;
	Phase_1->LevelToLoad = "Phase1_GamePlay";
	Phase_1->StartPhaseName = "P1";
	Phase_1->MainParentLevel = "Phase1Parent";
	
	Phase_2->State = EPhaseState::Phase_2;
	Phase_2->TimeLimit = 120.0f;
	Phase_2->LevelToLoad = "Phase2_GamePlay";
	Phase_2->StartPhaseName = "P2";
	Phase_2->MainParentLevel = "Phase2Parent";
	
	Phase_3->State = EPhaseState::Phase_3;
	Phase_3->TimeLimit = 180.0f;
	Phase_3->LevelToLoad = "Phase3_GamePlay";
	Phase_3->StartPhaseName = "P3";
	Phase_3->MainParentLevel = "Phase3Parent";

	Phases.Push(Phase_1);
	Phases.Push(Phase_2);
	Phases.Push(Phase_3);
	//Set Current Phase
	CurrentPhase = Phases[0];

	CurrentTime = 0;
	//UGameplayStatics::GetStreamingLevel(GetWorld(), (TEXT("%s"), *Phases[GetNextPhaseNumber()]->LevelToLoad))->SetShouldBeLoaded(true);
	bIsGameActive = true;

	TotalRounds = 3;
	if(TimerInfo != nullptr)
	{
		//if(GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Activate Time ");
		ATimerInfo::SetTime(TimeLimit);
		TimerInfo->SetIsActive(true);
	}

	TransitionComponent = NewObject<UTransitionComponent>(this);
	TransitionComponent->WidgetName = TEXT("ShowScore");
	OnloadedMap.AddDynamic(TransitionComponent, &UTransitionComponent::LoadingFinished);
	
	ARacketeersController* PController = Cast<ARacketeersController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	PController->OnPlayerPressedReady.AddDynamic(TransitionComponent, &UTransitionComponent::IncrementPlayerReady);

	TransitionComponent->OnFinished.AddDynamic(this, &ARacketeersGMBase::TravelToLevel);
	TransitionComponent->GameState = GetGameState<ARacketeersGameStateBase>();

	OnRoundFinish.AddDynamic(this, &ARacketeersGMBase::EndGame);
	
}




void ARacketeersGMBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(WidgetSubsystem != nullptr)
	{
		if(!WidgetSubsystem->CanRun)
		{
			return;
		}
	}
	if(CurrentPhase == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Returned NullPtr"));
		return;
	}
	//CurrentTime >= CurrentPhase->TimeLimit
	if(TimerInfo == nullptr && !TimerInfo->GetIsActive())
	{
		return;
	}
	if(ATimerInfo::GetTime() <= 0)
	{
		RoundCompletion();
	}else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current Time: %f"), CurrentTime);
		//CurrentTime += DeltaSeconds;
	}
}


void ARacketeersGMBase::RoundCompletion()
{
	CurrentTime = 0;

	TimerInfo->SetIsActive(false);

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Check If Game Is Over");
	CheckWinnerOfRound();
	if(CheckIfGameIsOver())
	{
		//if(GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "End Game");
		return;
	}
	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Check Winner of Round");
	SwitchIncomingState();

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Load Transition stats");
	//LoadTransitionStats();

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Transition");
	Transition();
}

void ARacketeersGMBase::TravelToLevel()
{
	//SwitchState();
	OnloadedMap.Broadcast();
	SetPackage();
	UnFreezePlayers();
	ProcessServerTravel(LevelToLoad, false);

}

bool ARacketeersGMBase::CheckWinnerOfRound()
{

	if(State == EPhaseState::Phase_3)
	{
		ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
		if(GS == nullptr) return false;


		if(GS->GetTeamStats(ETeams::TeamRaccoon).TeamAlive  == GS->GetTeamStats(ETeams::TeamPanda).TeamAlive )
		{

			float RaccoonTotalTeamHealth = 0;
			float PandasTotalTeamHealth = 0;
			for (APlayerState* RaccoonsAlive : GS->RaccoonsAlive)
			{
				APS_Base* PSBase = Cast<APS_Base>(RaccoonsAlive);
				if(PSBase)
				{
					RaccoonTotalTeamHealth += PSBase->BoatHealth;
				}
			}
			for (APlayerState* PandaAlive : GS->PandasAlive)
			{
				APS_Base* PSBase = Cast<APS_Base>(PandaAlive);
				if(PSBase)
				{
					PandasTotalTeamHealth += PSBase->BoatHealth;
				}
			}
			if(RaccoonTotalTeamHealth > PandasTotalTeamHealth)
			{
				GS->RacconsRoundsWon++;
				GS->RoundTeamWinner = ETeams::TeamRaccoon;
				return true;
			}
			GS->RedPandasRoundsWon++;
			GS->RoundTeamWinner = ETeams::TeamPanda;
			return true;
		}
		
		if(GS->GetTeamStats(ETeams::TeamRaccoon).TeamAlive > GS->GetTeamStats(ETeams::TeamPanda).TeamAlive)
		{
			GS->RacconsRoundsWon++;
			GS->RoundTeamWinner = ETeams::TeamRaccoon;
			return true;
		}
		if(GS->GetTeamStats(ETeams::TeamPanda).TeamAlive > GS->GetTeamStats(ETeams::TeamRaccoon).TeamAlive)
		{
			GS->RedPandasRoundsWon++;
			GS->RoundTeamWinner = ETeams::TeamPanda;
			return true;
		}
		
		GS->RacconsRoundsWon++;
		GS->RedPandasRoundsWon++;
	}
	return true;
}

void ARacketeersGMBase::SwitchState()
{
	if(CurrentPhase->State == EPhaseState::Phase_3)
	{
		CurrentPhase = Phases[0];
	}else
	{
		CurrentPhase = Phases[(int8)CurrentPhase->State + 1];	
	}

	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	if(GS == nullptr) {return;}
	GS->CurrentPhase = this->CurrentPhase->State;
	GS->OnRep_PhaseChange();
}

void ARacketeersGMBase::LoadEndLevel()
{
	ProcessServerTravel("VictoryMap_GamePlay");
}

void ARacketeersGMBase::LoadTransitionLevel()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "LOAD TRANSITION LEVEL");
	ProcessServerTravel("StatsTransitionMap",true);
}

void ARacketeersGMBase::Transition()
{
	
	OnloadedMap.Broadcast();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LevelToLoad);
	if(!PossibleLevelsToLoad.IsEmpty())
	{
		int RandomInt = FMath::RandRange(0, PossibleLevelsToLoad.Num() - 1);
		if(PossibleLevelsToLoad.IsValidIndex(RandomInt))
		{
			LevelToLoad = PossibleLevelsToLoad[RandomInt];
		}

	}
	SetPackage();
	FreezePlayers();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Transition");
	FTimerHandle TimerHandle;


	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	if(GS)
	{
		GS->MultiCastRoundEnded();	
	}
	GetWorldTimerManager().SetTimer(TimerHandle, this,  &ARacketeersGMBase::LoadTransitionLevel, TransitionTimer, false);
	//TransitionComponent->AddWidgetsToPlayers(GetGameState<ARacketeersGameStateBase>());

	
	
	/*
	/*
	if(CurrentPhase->State == EPhaseState::Phase_3)
	{
		return;
	}

	
	//LoadLevel();

	
	OnUnloadingMap.Broadcast();
	
	FLatentActionInfo ActionInfo;
	ActionInfo.Linkage = 0;
	ActionInfo.CallbackTarget = this;
	ActionInfo.ExecutionFunction = TEXT("LoadLevel");
	ActionInfo.UUID = GetUniqueID();	

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Unload Level");
	UnloadLevel((TEXT("%s"), *CurrentPhase->LevelToLoad), ActionInfo);
	*/
}

void ARacketeersGMBase::BroadcastOnPlayerPressed(ETeams Team)
{
	if(HasAuthority())
	{
		TransitionComponent->IncrementPlayerReady(Team);
	}
}

void ARacketeersGMBase::IncrementPlayerCounter()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "IncrementPlayerReady" );
	if(TransitionComponent->bIsOn)
	{
		TransitionComponent->CountPlayersReady++;
		AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "NUM: " + FString::FromInt(GS->PlayerArray.Num()) + " Current Player Count: " + FString::FromInt(TransitionComponent->CountPlayersReady) );
		if(GS == nullptr) return;
		
		if(TransitionComponent->bIsFinished && TransitionComponent->CountPlayersReady == GS->PlayerArray.Num())
		{
			TransitionComponent->CountPlayersReady = 0;
			TransitionComponent->OnFinished.Broadcast();
		}
	}
}

void ARacketeersGMBase::AllStagesFinished()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "ALL STAGES FINISHED" );
	//SwitchState(); 
	ATimerInfo::SetTime(TimeLimit);
	ATimerInfo::SetIsActive(true);
	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	if(GS)
	{
		GS->PandasReady = 0;
		GS->RaccoonsReady = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "ALL STAGES FINISHED GAME STATE" );
		//GS->ChangeCurrentPhase(CurrentPhase->State);
	}
	ARacketeersController* C = Cast<ARacketeersController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	//TransitionComponent->RemoveWidgetsFromPlayers();
	//TransitionComponent->CountPlayersReady = 0;

	//RespawnPlayers();
	if(C->HasAuthority())
	{
		C->ServerMultiCastActivateTimer();
	}
}

int ARacketeersGMBase::GetNextPhaseNumber()
{
	if(CurrentPhase->State == EPhaseState::Phase_3)
	{
		return 0;
	}
	else
	{
		return (int8)CurrentPhase->State + (int8)1;
	}
	
}

struct FTeamMatch
{
	ETeams Team = ETeams::NONE;
	int32 RoundsWon = 0;
};


bool ARacketeersGMBase::CheckIfGameIsOver()
{

	ARacketeersGameStateBase* GS = this->GetGameState<ARacketeersGameStateBase>();
	
	FTeamMatch Raccoons(ETeams::TeamRaccoon, GS->RacconsRoundsWon);
	FTeamMatch Pandas(ETeams::TeamPanda, GS->RedPandasRoundsWon);

	FTeamMatch Leader = Raccoons.RoundsWon > Pandas.RoundsWon ? Leader = Raccoons : Leader = Pandas;
	
	if(State == EPhaseState::Phase_3)
	{
		
		int32 AvailibleRounds = TotalRounds - (GS->RacconsRoundsWon + GS->RedPandasRoundsWon);
		int8 RoundsPlayed = GS->RacconsRoundsWon + GS->RedPandasRoundsWon;

		if(Leader.RoundsWon > AvailibleRounds)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LEADER WINS");
			OnRoundFinish.Broadcast();
			return true;
		}
	}
	return false;
	
}

bool ARacketeersGMBase::LoadTransitionStats()
{

	//New Transition Way
	TransitionComponent->AddWidgetsToPlayers(GetGameState<AGameStateBase>());
	
	
	//bIsGameActive = false;
	
	//WidgetSubsystem->OnLoadWidget.Broadcast("ShowScore");
	//WidgetSubsystem->SetCanRunTick(false);
	//OnLoadWidget.Broadcast();
	return true;
	
}
void ARacketeersGMBase::EndGame()
{
	SetPackage();
	//Stop Players

	//Transition
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this,  &ARacketeersGMBase::LoadEndLevel, TransitionTimer, false);
	//ProcessServerTravel("VictoryMap_GamePlay");
}

void ARacketeersGMBase::IncreaseTotalRounds()
{
	if(TotalRounds + 1 > MAXTOTALROUNDS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached Max Total Rounds"));
		return;
	}
	TotalRounds++;
}

void ARacketeersGMBase::DecreaseTotalRounds()
{
	if(TotalRounds - 1 < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached under valid Total Rounds "));
		return;
	}
	TotalRounds--;
}

int8 ARacketeersGMBase::GetTotalRounds()
{
	return TotalRounds;
}

EPhaseState ARacketeersGMBase::SwitchIncomingState()
{
	EPhaseState NewPhase = CurrentPhase->State;
	if(NewPhase == EPhaseState::Phase_3)
	{
		NewPhase = EPhaseState::Phase_1;
	}else
	{
		NewPhase = Phases[(int8)CurrentPhase->State + (int8)1]->State;
	}
	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	GS->IncomingPhase = NewPhase;
	GS->OnRep_IncomingPhaseChange();

	return NewPhase;
}

void ARacketeersGMBase::SetPackage()
{
	UBaseGameInstance* BI = Cast<UBaseGameInstance>(GetGameInstance());
	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();

	int32 ExpectedRaccoons = 0;
	int32 ExpecedPandas = 0;
	for (APlayerState* PlayerState : GetGameState<AGameState>()->PlayerArray)
	{
		APS_Base* APS = Cast<APS_Base>(PlayerState);
		if(APS)
		{
			if(APS->PlayerInfo.Team == ETeams::TeamRaccoon)
			{
				ExpectedRaccoons++;
				continue;
			}
			ExpecedPandas++;
		}
	}
	
	FGameStatsPackage Package{
		GS->RacconResource,
		GS->RacconsRoundsWon,
		GS->RaccoonsBoatHealth,
		GS->RaccoonParts,
		GS->RaccoonCraftingProgress,
		GS->RedPandasResource,
		GS->RedPandasRoundsWon,
		GS->RedPandasBoatHealth,
		GS->PandasParts,
		GS->PandaCraftingProgress,
		ExpectedRaccoons,
		ExpecedPandas
	};

	if(Package.RacconsRoundsWon > Package.RedPandasRoundsWon)
	{
		Package.WonTeam = ETeams::TeamRaccoon;
	}
	else if (Package.RacconsRoundsWon < Package.RedPandasRoundsWon)
	{
		Package.WonTeam = ETeams::TeamPanda;
	}
	else
	{
		Package.WonTeam = ETeams::NONE;
	}


	if(BI == nullptr) return;
	FGameModeData Data;
	Data.LevelToLoad = LevelToLoad;
	Data.Phase = State;
	BI->SetGameModeData(Data);
	BI->SetDataToTransfer(Package);
	
}

void ARacketeersGMBase::SetSpectatingPlayersToPlayers()
{
	AGameStateBase* GS = GetGameState<AGameState>();
	if(GS == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Game State is Nullptr, ARacketeersGMBase::SetSpectatingPlayersToPlayers "));
	}
	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		if(PlayerState && PlayerState->IsOnlyASpectator())
		{
			PlayerState->SetIsOnlyASpectator(false);
			ARacketeersController* AC = Cast<ARacketeersController>(PlayerState->GetPlayerController());
			if(AC)
			{
				AC->SetPlayerPlay();
			}
		}	
	}
	
}

void ARacketeersGMBase::MakeControllerSpectator(APlayerController* PC)
{
	if(PC)
	{
		PC->ChangeState(NAME_Spectating);
		PC->ClientGotoState(NAME_Spectating);
	}
}

void ARacketeersGMBase::UnloadLevel(FName name, FLatentActionInfo& ActionInfo)
{
	UGameplayStatics::UnloadStreamLevel(GetWorld(), name, ActionInfo, false);
}

void ARacketeersGMBase::LoadLevel()
{
	
	FLatentActionInfo LoadActionInfo;
	
	LoadActionInfo.Linkage = 0;
	LoadActionInfo.CallbackTarget = this;
	LoadActionInfo.ExecutionFunction = TEXT("RespawnPlayers");
	LoadActionInfo.UUID = GetUniqueID();

	if(CurrentPhase->State == EPhaseState::Phase_2)
	{
		
	}

	bool bStreamingSucceded = false;

	/*
	LevelLoadingManager->UnloadLevel();
	
	LevelLoadingManager->OnLoadingLevelCompleted.AddDynamic(this, &ARacketeersGMBase::RespawnPlayers);
	LevelLoadingManager->NextSubLevelPath = Phases[GetNextPhaseNumber()]->LevelToLoad;
	LevelLoadingManager->LoadLevel();
	for (TObjectPtr<APlayerState> PlayerState : GameState->PlayerArray) 
	{
		ARacketeersController* PC = Cast<ARacketeersController>(PlayerState->GetPlayerController());
		PC->ClientUnLoadLevel(CurrentPhase->LevelToLoad);
		PC->ClientLoadLevel(LevelLoadingManager->CurrentSubLevelPath);
	}
	*/

	
	//ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), *Phases[GetNextPhaseNumber()]->LevelToLoad, FVector::ZeroVector,FRotator::ZeroRotator,bStreamingSucceded);
	//RespawnPlayers();
	UGameplayStatics::LoadStreamLevel(GetWorld(), *Phases[GetNextPhaseNumber()]->LevelToLoad, true,false, LoadActionInfo);
}

void ARacketeersGMBase::RespawnPlayers()
{
	UWorld* World = GetWorld();

	// If incoming start is specified, then just use it
	
	
	for (int i = 0; i < this->GetGameState<AGameState>()->PlayerArray.Num(); ++i)
	{
		APS_Base* PS = Cast<APS_Base>(this->GetGameState<AGameState>()->PlayerArray[i]);
		FString TeamName;
		TeamName.AppendInt((int32)PS->PlayerInfo.Team);
		TeamName.AppendInt(PS->PlayerInfo.TeamPlayerID);
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *TeamName);

		AActor* PlayerStart = FindPlayerStart(PS->GetPlayerController(),TeamName);


		
			
		UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *TeamName);
		PS->GetPawn()->SetActorLocation(PlayerStart->GetActorLocation());
		PS->GetPawn()->SetActorRotation(PlayerStart->GetActorRotation());
		
	}
	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Respawn Players");
	
}

void ARacketeersGMBase::RespawnPlayer(APlayerState* PState)
{
	if(PState == nullptr) return;
	APS_Base* PS = Cast<APS_Base>(PState);
	FString TeamName;

	TeamName.AppendInt((int32)PS->PlayerInfo.Team);
	TeamName.AppendInt(PS->PlayerInfo.TeamPlayerID);
	
	AActor* PlayerStart = FindPlayerStart(PS->GetPlayerController(),TeamName);
	if(PlayerStart == nullptr || PS == nullptr || PS->GetPawn() == nullptr) return;
	PS->GetPawn()->SetActorLocation(PlayerStart->GetActorLocation());
	PS->GetPawn()->SetActorLocation(FVector(PS->GetPawn()->GetActorLocation().X, PS->GetPawn()->GetActorLocation().Y, 0.0f), true);

	
}

/*
 * CLass Sync
 *  Send Info about what it should run when finished
 *  
 *	
 *	
 *	
 */
/*void ARacketeersGMBase::InitializeBoatData()
{
	// Raccoon Team
	FTeamParts RaccoonParts;
	RaccoonParts.PlayerValues.Add(1, {90, 500, 1, RaccoonHullMaterial, RaccoonSailMaterial, BoatRaccoon,"Raccoon0"});
	RaccoonParts.PlayerValues.Add(2, {180, 500, 2, RaccoonHullMaterial, RaccoonSailMaterial, BoatRaccoon,"Raccoon1"});
	RaccoonParts.PlayerValues.Add(3, {360, 500, 3, RaccoonHullMaterial, RaccoonSailMaterial, BoatRaccoon,"Raccoon2"});

	// Panda Team
	FTeamParts PandaParts;
	PandaParts.PlayerValues.Add(1, {90, 500, 1,PandaHullMaterial, PandaSailMaterial, BoatPanda,"Panda0"});
	PandaParts.PlayerValues.Add(2, {180, 500, 2, PandaHullMaterial, PandaSailMaterial, BoatPanda,"Panda1"});
	PandaParts.PlayerValues.Add(3, {360, 500, 3, PandaHullMaterial, PandaSailMaterial, BoatPanda,"Panda2"});

	// Add to TeamData
	TeamData.Add(ETeams::Team_Raccoon, RaccoonParts);
	TeamData.Add(ETeams::Team_Panda, PandaParts);
} 

void ARacketeersGMBase::GetActivePlayers()
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

void ARacketeersGMBase::SetBoatValues(ETeams MyTeamID, int32 MyPlayerID)
{
	BoatCharacter = Cast<ABoatCharacter>(UGameplayStatics::GetPlayerPawn(this, PlayerID - 1));
	if (!BoatCharacter) return;

	if (TeamData.Contains(static_cast<ETeams>(TeamID)))
	{
		FTeamParts& TeamParts = TeamData[static_cast<ETeams>(TeamID)];
		if (TeamParts.PlayerValues.Contains(PlayerID))
		{
			const FBoatData& BoatValues = TeamParts.PlayerValues[PlayerID];
			BoatCharacter->SetHealth(BoatValues.Health);
			BoatCharacter->SetCannonAmount(BoatValues.CannonAmount);
			BoatCharacter->SetHullAndSailMaterial(BoatValues.HullMaterial, BoatValues.SailMaterial);
		}
	}
}
*/