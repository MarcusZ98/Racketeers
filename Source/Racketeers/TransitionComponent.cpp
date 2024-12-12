// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionComponent.h"

#include "RacketeersController.h"
#include "RacketeersGMBase.h"
#include "WidgetSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


class ARacketeersGMBase;
class AGameState;
// Sets default values for this component's properties
UTransitionComponent::UTransitionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	
	// ...
}


// Called when the game starts
void UTransitionComponent::BeginPlay()
{
	Super::BeginPlay();
	SetWidgetWithName(WidgetName);
	// ...
	
}

void UTransitionComponent::IncrementPlayerReady(ETeams Team)
{
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "IncrementPlayerReady" );
		CountPlayer(Team);
		AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "NUM: " + FString::FromInt(GS->PlayerArray.Num()) + " Current Player Count: " + FString::FromInt(CountPlayersReady) );

		if(GS == nullptr) return;
		if(CountPlayersReady == GS->PlayerArray.Num())
		{
			CountPlayersReady = 0;
			OnFinished.Broadcast();
		}
	
	//ARacketeersGMBase* GM = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
	//GM->IncrementPlayerCounter();
}

void UTransitionComponent::CountPlayer(ETeams Team)
{
	ARacketeersGameStateBase* GS = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	CountPlayersReady++;
	if(GS == nullptr)
	{

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "GameState IS NULLPTR, CountPlayer");
		return;
	}

	if(Team == ETeams::TeamRaccoon)
	{
		GS->RaccoonsReady++;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Add  to RaccoonsReady");
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Add  to PandasReady");
	GS->PandasReady++;
}

// Called every frame
void UTransitionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTransitionComponent::SetWidgetWithName(FName Name)
{
	UWidgetSubsystem* WS = GetWorld()->GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if(WS == nullptr)
	{
		return;
	}
	WidgetName = Name;
	//WS->ActiveWidgetComponents.Find(Name);
}

void UTransitionComponent::AddWidgetsToPlayers(AGameStateBase* GS)
{
	bIsFinished = false;
	bIsOn = true;
	OnBeginTransition.Broadcast();
	
	if(GS == nullptr)
	{
		if(GEngine)
		return;
	}
	for (APlayerState* PlayerState : GS->PlayerArray)
	{
	
		if(PlayerState == nullptr)
		{
			if(GEngine)

			continue;
		}
		ARacketeersController* PC = Cast<ARacketeersController>(PlayerState->GetPlayerController());
		if (PC == nullptr)
		{
			if(GEngine)
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, "PLAYER CONTROLLER STATE IS A NULLPTR");
			continue;
		}
		if(GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, "CALL TO ACTIVATE WIDGET");
		PC->ActivateWidget(WidgetName, WidgetToDisplay);
	}
}

void UTransitionComponent::RemoveWidgetsFromPlayers()
{
	for (APlayerState*  PlayerState : UGameplayStatics::GetGameState(GetWorld())->PlayerArray)
	{
		ARacketeersController* PC = Cast<ARacketeersController>(PlayerState->GetPlayerController());
		if (PC == nullptr) continue;
		PC->RemoveWidget(WidgetName);
	}
}

void UTransitionComponent::LoadingFinished()
{
	//bIsOn = false;
	bIsFinished = true;
	if(GameState == nullptr){
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "On Finished");
		return;
	}
	if(CountPlayersReady == GameState->PlayerArray.Num())
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "On Finished");
		OnFinished.Broadcast();
	}
}

