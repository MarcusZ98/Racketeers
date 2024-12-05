// Fill out your copyright notice in the Description page of Project Settings.


#include "RacketeersGMBase.h"

#include "BaseGameInstance.h"
#include "HeadMountedDisplayTypes.h"
#include "PS_Base.h"
#include "RacketeersController.h"
#include "RacketeersGameStateBase.h"
#include "TransitionComponent.h"
#include "WidgetSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/GameStateBase.h"
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
	//UE_LOG(LogTemp, Warning, TEXT("RacketeersGMBase::BeginPlay"));
	
	//Set the GameState in GameMode
	//GameState = Cast<AGS_Base>(UGameplayStatics::GetGameState(GetWorld()));


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
		ATimerInfo::SetTime(Phase_1->TimeLimit);
		TimerInfo->SetIsActive(true);
	}

	TransitionComponent = NewObject<UTransitionComponent>(this);
	TransitionComponent->WidgetName = TEXT("ShowScore");
	OnloadedMap.AddDynamic(TransitionComponent, &UTransitionComponent::LoadingFinished);
	
	ARacketeersController* PController = Cast<ARacketeersController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	PController->OnPlayerPressedReady.AddDynamic(TransitionComponent, &UTransitionComponent::IncrementPlayerReady);

	TransitionComponent->OnFinished.AddDynamic(this, &ARacketeersGMBase::AllStagesFinished);
	TransitionComponent->GameState = GetGameState<ARacketeersGameStateBase>();
	
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
	if(!TimerInfo->GetIsActive())
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
	if(CheckIfGameIsOver())
	{
		//if(GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "End Game");
		EndGame();
		return;
	}
	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Check Winner of Round");
	SwitchIncomingState();
	CheckWinnerOfRound();

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Load Transition stats");
	LoadTransitionStats();

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Transition");
	Transition();
}

bool ARacketeersGMBase::CheckWinnerOfRound()
{

	if(CurrentPhase->State == EPhaseState::Phase_3)
	{
		ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
		if(GS == nullptr) return false;
		if(GS->GetTeamStats(ETeams::Team_Raccoon).TeamAlive > GS->GetTeamStats(ETeams::Team_Panda).TeamAlive)
		{
			GS->RacconsRoundsWon++;
			return true;
		}
		if(GS->GetTeamStats(ETeams::Team_Panda).TeamAlive > GS->GetTeamStats(ETeams::Team_Raccoon).TeamAlive)
		{
			GS->RedPandasRoundsWon++;
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
		CurrentPhase = Phases[CurrentPhase->State+1];	
	}
	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	GS->CurrentPhase = this->CurrentPhase->State;
	GS->OnRep_PhaseChange();
}


void ARacketeersGMBase::Transition()
{


	LoadLevel();

	/*
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
		ARacketeersController* PController = Cast<ARacketeersController>(	UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PController->OnPlayerPressedReady.Broadcast(Team);
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
	SwitchState(); 
	ATimerInfo::SetTime(CurrentPhase->TimeLimit);
	ATimerInfo::SetIsActive(true);
	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	if(GS)
	{
		GS->PandasReady = 0;
		GS->RaccoonsReady = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "ALL STAGES FINISHED GAME STATE" );
		GS->ChangeCurrentPhase(CurrentPhase->State);
	}

	
	ARacketeersController* C = Cast<ARacketeersController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TransitionComponent->RemoveWidgetsFromPlayers();
	TransitionComponent->CountPlayersReady = 0;
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
		return CurrentPhase->State+1;
	}
	
}



bool ARacketeersGMBase::CheckIfGameIsOver()
{

	ARacketeersGameStateBase* GS = this->GetGameState<ARacketeersGameStateBase>();
	
	if(CurrentPhase->State == EPhaseState::Phase_3)
	{
		int AvailibleRounds = TotalRounds - (GS->RacconsRoundsWon + GS->RedPandasRoundsWon);
		int8 RoundsPlayed = GS->RacconsRoundsWon + GS->RedPandasRoundsWon;
		if(RoundsPlayed >= GetTotalRounds())
		{
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

bool ARacketeersGMBase::EndGame()
{

	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	
	FGameStatsPackage Package{
		GS->RacconResource,
		GS->RacconsRoundsWon,
		GS->RaccoonsBoatHealth,
		GS->RedPandasResource,
		GS->RedPandasRoundsWon,
		GS->RedPandasBoatHealth
	};

	if(Package.RacconsRoundsWon > Package.RedPandasRoundsWon)
	{
		Package.WonTeam = ETeams::Team_Raccoon;
	}
	else if (Package.RacconsRoundsWon < Package.RedPandasRoundsWon)
	{
		Package.WonTeam = ETeams::Team_Raccoon;
	}

	Package.WonTeam = ETeams::NONE;

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	GI->SetDataToTransfer(Package);
	ProcessServerTravel("VictoryMap_GamePlay");

	return true;
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

TEnumAsByte<EPhaseState> ARacketeersGMBase::SwitchIncomingState()
{
	TEnumAsByte<EPhaseState> NewPhase = CurrentPhase->State;
	if(NewPhase == EPhaseState::Phase_3)
	{
		NewPhase = EPhaseState::Phase_1;
	}else
	{
		NewPhase = Phases[CurrentPhase->State+1]->State;
	}
	ARacketeersGameStateBase* GS = GetGameState<ARacketeersGameStateBase>();
	GS->IncomingPhase = NewPhase;
	GS->OnRep_IncomingPhaseChange();

	return NewPhase;
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
	LevelLoadingManager->MulticastLoadLevel(Phases[GetNextPhaseNumber()]);
	LevelLoadingManager->OnLoadingLevelCompleted.AddDynamic(this, &ARacketeersGMBase::RespawnPlayers);

	
	//ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), *Phases[GetNextPhaseNumber()]->LevelToLoad, FVector::ZeroVector,FRotator::ZeroRotator,bStreamingSucceded);
	//RespawnPlayers();
	//UGameplayStatics::LoadStreamLevel(GetWorld(), *Phases[GetNextPhaseNumber()]->LevelToLoad, true,false, LoadActionInfo);
}

void ARacketeersGMBase::RespawnPlayers()
{
	
	for (int i = 0; i < this->GetGameState<AGameState>()->PlayerArray.Num(); ++i)
	{
		APS_Base* PS = Cast<APS_Base>(this->GetGameState<AGameState>()->PlayerArray[i]);
		FString TeamName;
		if(PS->PlayerInfo.Team == ETeams::Team_Raccoon)
		{
			TeamName ="Team Raccoon";
		}
		else if(PS->PlayerInfo.Team == ETeams::Team_Panda)
		{
			TeamName ="Team Panda";
		}
		TeamName.AppendInt(PS->PlayerInfo.TeamPlayerID);
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *TeamName);

		AActor* PlayerStart = FindPlayerStart(PS->GetPlayerController(),TeamName);


		UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *TeamName);
		PS->GetPawn()->SetActorLocation(PlayerStart->GetActorLocation());
		PS->GetPawn()->SetActorRotation(PlayerStart->GetActorRotation());
	}
	TransitionComponent->bIsFinished = true;
	OnloadedMap.Broadcast();
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Respawn Players");
	
}

void ARacketeersGMBase::RespawnPlayer(APlayerState* PState)
{
	if(PState == nullptr) return;
	APS_Base* PS = Cast<APS_Base>(PState);
	FString TeamName;

	if(PS->PlayerInfo.Team == ETeams::Team_Raccoon)
	{
		TeamName ="Team Raccoon";
	}
	else if(PS->PlayerInfo.Team == ETeams::Team_Panda)
	{
		TeamName ="Team Panda";
	}
	TeamName.AppendInt(PS->PlayerInfo.TeamPlayerID);
	
	AActor* PlayerStart = FindPlayerStart(PS->GetPlayerController(),TeamName);
	if(PlayerStart == nullptr || PS == nullptr || PS->GetPawn() == nullptr) return;
	PS->GetPawn()->SetActorLocation(PlayerStart->GetActorLocation());
	
}

