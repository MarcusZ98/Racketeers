// Fill out your copyright notice in the Description page of Project Settings.


#include "RacketeersController.h"
#include <string>

#include "OnlineSubsystemUtils.h"
#include "Blueprint/UserWidget.h"
#include "RacketeersGameStateBase.h"
#include "RacketeersGMBase.h"
#include "WidgetSubsystem.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Widget.h"



void ARacketeersController::Call_Interact_Implementation(const FString &string)
{
	//Change Player Enum Trough out Servers and send it
	//UE_LOG(LogTemp, Warning, TEXT("Call_Interact_Implementation: %ls"), *string);

	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *string);
}

bool ARacketeersController::Call_Interact_Validate(const FString &string)
{
	return true;
}


void ARacketeersController::DamageBoat_Implementation(int Amount, ETeams Team)
{
	ARacketeersGameStateBase* State = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(State == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("Could Not DamageBoat in ARacketeersController"));
		return;
	}
	State->DamageBoat(Amount, Team);
}

bool ARacketeersController::DamageBoat_Validate(int Amount, ETeams Team)
{
	return true;
}

void ARacketeersController::ActivateWidget_Implementation(FName Name, UUserWidget* Widget)
{
	bhavePressedContinue = false;
	UWidgetSubsystem* WS = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if(!WS->ActiveWidgetComponents.Contains(Name) && WS->MapOfAllPlayerWidget.Contains(Name))
	{
		
		UUserWidget* W = *WS->MapOfAllPlayerWidget.Find(Name);
		if(W)
		{
		    W->AddToViewport();
			WS->ActiveWidgetComponents.Add(Name, W);
		}
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Could Not Activate Widget");
	//UserWidget = Widget;
	
	/*
	int32 s = GetUniqueID();
	FString String = FString::FromInt(s);

	bhavePressedContinue = false;
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *String);
	if(Widget == nullptr)
	{
		return;
	}
	
	UWidgetSubsystem* WS = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if(WS == nullptr)
	{
		return;
	}
	if(!WS->ActiveWidgetComponents.Contains(Name))
	{
		Widget->AddToViewport();
		WS->ActiveWidgetComponents.Add(Name, Widget);
	}
	//UserWidget = Widget;
	*/
}

void ARacketeersController::RemoveWidget_Implementation(FName Name)
{
	UWidgetSubsystem* WS = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if(WS == nullptr)
	{
		return;
	}
	if(WS->ActiveWidgetComponents.Contains(Name))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, "IT DOES CONTAIN");
		UUserWidget* Widget= *WS->ActiveWidgetComponents.Find(Name);
		if(Widget)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, "REMOVE FROM PARENT");
			Widget->RemoveFromParent();
			WS->ActiveWidgetComponents.Remove(Name);
			bhavePressedContinue = false;
		}
	
	}


	//UserWidget->RemoveFromParent();
	
}

void ARacketeersController::RequestRemoveWidget_Implementation()
{
	
	if(bhavePressedContinue)
	{
		return;
	}
	UWidgetSubsystem* WS = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	if(WS == nullptr)
	{
		return;
	}
	if(WS->ActiveWidgetComponents.Num() == 0)
	{
		return;
	}
	ARacketeersGameStateBase* State = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(State == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not DamageBoat in ARacketeersController"));
		return;
	}
	bhavePressedContinue = true;
	State->RequestToRemoveWidget();
}

bool ARacketeersController::RequestRemoveWidget_Validate()
{
	return true;
}


void ARacketeersController::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "ARacketeersController::BeginPlay");
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	OnBeginPlayerEvent.Broadcast();
	PauseMenuWidget = CreateWidget(this,PauseWidgetClass,"PauseMenu");
}

void ARacketeersController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Pause", IE_Pressed, this, &ARacketeersController::OpenPauseMenu);
	//EnhancedInputComponent->BindAction(
}

void ARacketeersController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	
	//DOREPLIFETIME(ARacketeersController, bhavePressedContinue);
	
}

void ARacketeersController::ServerPlayerLeave_Implementation(APlayerController* PC)
{
	if(PC)
	{
		if(PC->GetLocalRole() == ROLE_Authority)
		{
			UGameplayStatics::GetGameMode(GetWorld())->GameSession->Destroy();
			return;
		}
		UGameplayStatics::GetGameMode(GetWorld())->GameSession->KickPlayer(PC, FText::GetEmpty());
		PC->Destroy();
		
	}
}

bool ARacketeersController::ServerPlayerLeave_Validate(APlayerController* PC)
{
	return true;
}

void ARacketeersController::SetPlayerSpectator()
{
	if(!HasAuthority())
	{
		return;
	}
	/*
	if(GetPawn())
	{
		GetPawn()->Destroy();
	}
	*/
	//GetPawn()->Destroy();
	PlayerState->SetIsSpectator(true);
	//ChangeState(NAME_Spectating);
	bPlayerIsWaiting = true;
	//ClientGotoState(NAME_Spectating);
}

void ARacketeersController::SetPlayerPlay()
{
	if(!HasAuthority())
	{
		return;
	}
	PlayerState->SetIsSpectator(false);
	ChangeState(NAME_Playing);
	bPlayerIsWaiting = false;
	ClientGotoState(NAME_Playing);
}

void ARacketeersController::OpenPauseMenu(){
	if(PauseMenuWidget == nullptr && PauseMenuWidget != nullptr	)
	{
		PauseMenuWidget = CreateWidget(this,PauseWidgetClass,"PauseMenu");
	}
	// if(PauseMenuWidget->IsInViewport() && PauseMenuWidget!=nullptr)
	// {
	// 	PauseMenuWidget->RemoveFromParent();
	// 	SetInputMode(FInputModeGameOnly());
	// }
	else
	{
		PauseMenuWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly());
		SetShowMouseCursor(true);
	}
}

void ARacketeersController::ClientUnLoadLevel_Implementation(const FString& LevelPath)
{
	for (ULevelStreaming* StreamingLevel : GetWorld()->GetStreamingLevels())
	{
		if(StreamingLevel->GetName() == LevelPath)
		{
			StreamingLevel->SetShouldBeVisible(false);
			StreamingLevel->SetShouldBeLoaded(false);
			StreamingLevel->SetIsRequestingUnloadAndRemoval(true);
			return;
		}
	}
}

bool ARacketeersController::ClientUnLoadLevel_Validate(const FString& LevelPath)
{
	return true;
}

void ARacketeersController::ClientLoadLevel_Implementation(const FString& LevelPath)
{
	bool bLevelLoadSuccessfull;
	ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), LevelPath, FVector::ZeroVector,FRotator::ZeroRotator,bLevelLoadSuccessfull);
	if(!bLevelLoadSuccessfull)
	{
		ClientLoadLevel(FString(LevelPath));
	}
}

bool ARacketeersController::ClientLoadLevel_Validate(const FString& LevelPath)
{
	return true;
}

void ARacketeersController::ServerRespawnPlayer_Implementation(APlayerState* PSState)
{
	if(HasAuthority())
	{
		ARacketeersGMBase* GMBase = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));

		if(GMBase == nullptr) return;
		GMBase->RespawnPlayer(PSState);
	}
}

bool ARacketeersController::ServerRespawnPlayer_Validate(APlayerState* PSState)
{
	return true;
}


void ARacketeersController::AddPart_Implementation(ETeams Team, EPartSpacing Part, int32 NewPart)
{
	ARacketeersGameStateBase* State = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(State == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("Game State is NULLPTR in AddTOStats_Implementation in RackeetersController"));
		return;
	}
	State->AddPart(Team, Part, NewPart);
}

bool ARacketeersController::AddPart_Validate(ETeams Team, EPartSpacing Part, int32 NewPart)
{
	return true;
}

void ARacketeersController::AddToStats_Implementation(int Amount, EGameStats GameStats, ETeams Team)
{
	ARacketeersGameStateBase* State = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(State == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Game State is NULLPTR in AddTOStats_Implementation in RackeetersController"));
		return;
	}
	State->AddToStats(Amount, GameStats, Team);
	
}

bool ARacketeersController::AddToStats_Validate(int Amount, EGameStats GameStats, ETeams Team)
{
	return true;
}

void ARacketeersController::ClientCheckReady_Implementation(ETeams Team)
{
	//if(bhavePressedContinue) return;
	//bhavePressedContinue = true;
	ServerCheckReady(Team);
}

bool ARacketeersController::ClientCheckReady_Validate(ETeams Team)
{
	return true;
}

void ARacketeersController::ServerCheckReady_Implementation(ETeams Team)
 {
	if(!HasAuthority()) return;
	ARacketeersGMBase* GM = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "SERVER CHECK READY");

	
	GM->BroadcastOnPlayerPressed(Team);
 }
 
 bool ARacketeersController::ServerCheckReady_Validate(ETeams Team)
 {
	return true;
 }

void ARacketeersController::ServerMultiCastActivateTimer_Implementation()
{
	MultiCastActivateTimer(ATimerInfo::GetTime(), ATimerInfo::GetIsActive());

	for (APlayerState* PState : UGameplayStatics::GetGameState(GetWorld())->PlayerArray)
	{
		ARacketeersController* PController = Cast<ARacketeersController>(PState->GetPlayerController());
		
		PController->SetTimeSeconds(ATimerInfo::GetTime(), ATimerInfo::GetIsActive());
	}
}

void ARacketeersController::MultiCastActivateTimer_Implementation(float T, bool SetIsActive)
{

	ATimerInfo::SetIsActive(SetIsActive);
	ATimerInfo::SetTime(T);
	SetTimeSeconds(T, SetIsActive);
}

void ARacketeersController::SetTimeSeconds_Implementation(float seconds, bool SetIsActive)
{
	
	ATimerInfo::SetTime(seconds);
	ATimerInfo::SetIsActive(SetIsActive);
}

void ARacketeersController::SetTime_Analog_Implementation(ATimerInfo* timer, int32 Minutes, int32 Seconds, bool SetIsActive)
{
	
}

void ARacketeersController::AddWidgetToViewport_Implementation(UUserWidget* Widget)
{
	if(Widget == nullptr)
	{
		return;
	}
	Widget->AddToViewport();
}

void ARacketeersController::RemoveResource_Implementation(int Amount, EResources Resource, ETeams Team)
{
	ARacketeersGameStateBase* State = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (State == nullptr)
	{
		return;
	}
	State->RemoveResource(Amount, Resource, Team);
}

bool ARacketeersController::RemoveResource_Validate(int Amount, EResources Resource, ETeams Team)
{
	return true;
}

void ARacketeersController::AddResource_Implementation(int Amount, EResources Resource, ETeams Team)
{
	ARacketeersGameStateBase* State = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (State == nullptr)
	{
		return;
	}
	if(GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "TOTAL AMOUNT TO ADD : " +  FString::FromInt(Amount));
	}
	State->AddResource(Amount, Resource, Team);
}

bool ARacketeersController::AddResource_Validate(int Amount, EResources Resource, ETeams Team)
{
	return true;
}

void ARacketeersController::SetServerTimeSeconds_Implementation(ARacketeersController* Controller ,float seconds, bool SetIsActive)
{
	ARacketeersGMBase* GM = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(GM == nullptr)
	{
		return;
	}
	Controller->SetTimeSeconds(ATimerInfo::GetTime(), ATimerInfo::GetIsActive());
	
}

