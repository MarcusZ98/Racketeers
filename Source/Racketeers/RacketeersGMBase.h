// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoatCharacter.h"
#include "GM_Base.h"
#include "LevelLoadingManager.h"
#include "TimerInfo.h"
#include "TransitionComponent.h"
#include "WidgetSubsystem.h"
#include "RacketeersGMBase.generated.h"

/**
 * Varje Start måste ha rätt phase tag, sen kan det också behövas att de olika starts får ett id för spelare och team.
 *
 *
 * IDAG,
 *
 * FIXA KLART WIDGET, Se till att poängen räknas ihopp av alla spelare. 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadWidget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadingMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnloadedMap);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundFinish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadTransitionMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadEndGame);

#define MAXTOTALROUNDS 8

UCLASS()
class RACKETEERS_API ARacketeersGMBase : public AGM_Base
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLoadTransitionMap OnLoadTransitionMap;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLoadEndGame OnLoadEndGame;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnRoundFinish OnRoundFinish;

	ARacketeersGMBase();
	FTimerDynamicDelegate OnPlayerControllerConstructed;

	UFUNCTION()
	void PlayerControllerConstructed();
	
	//Events / Delegates
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;

	virtual void AddInactivePlayer(APlayerState* PlayerState, APlayerController* PC) override;

	UPROPERTY()
	TArray<APlayerController*> JoiningPlayersControllers;
	UPROPERTY()
	TArray<APlayerState*> JoiningPlayerStates;
	
	UFUNCTION(BlueprintCallable)
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player);
	UFUNCTION(BlueprintCallable)
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	
	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadWrite)
	ATimerInfo* TimerInfo = nullptr;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadWrite)
	ALevelLoadingManager* LevelLoadingManager = nullptr;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadWrite)
	UTransitionComponent* TransitionComponent = nullptr;
	/*
	 * When A Widget Need To Load
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLoadWidget OnLoadWidget;
	/*
	* When A Widget Need To Unload
	*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLoadWidget OnUnloadWidget;
	/*
	* When the game mode has loaded a new map
	*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnloadedMap OnloadedMap;
	/*
	* When the game mode starts to unload a new map
	*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUnloadingMap OnUnloadingMap;

	UPROPERTY()
	UWidgetSubsystem* WidgetSubsystem;
	UPROPERTY()
	int8 UnloadWidgetCount;
	
	UFUNCTION(BlueprintCallable)
	void UnloadWidget();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	
	UPROPERTY(Editanywhere, BlueprintReadWrite, Blueprintable, Category = "Phases")
	UPhase* Phase_1;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Blueprintable, Category = "Phases")
	UPhase* Phase_2;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Blueprintable, Category = "Phases")
	UPhase* Phase_3;


	UFUNCTION(BlueprintCallable)
	void LoadLevel();
	UFUNCTION(BlueprintCallable)
	void UnloadLevel(FName name, FLatentActionInfo& ActionInfo);
	
	UFUNCTION(BlueprintCallable)
	void RespawnPlayers();


	
	UFUNCTION(Blueprintable, BlueprintCallable)
	virtual void EndGame();
	
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer(APlayerState* PState);
	UFUNCTION(BlueprintCallable)
	void IncreaseTotalRounds();
	UFUNCTION(BlueprintCallable)
	void DecreaseTotalRounds();
	UFUNCTION(BlueprintCallable)
	void RoundCompletion();
	UFUNCTION(BlueprintCallable)
	void TravelToLevel();
	UFUNCTION(BlueprintCallable)
	void Transition();
	void BroadcastOnPlayerPressed(ETeams Team);
	void IncrementPlayerCounter();
	int8 GetTotalRounds();
	EPhaseState SwitchIncomingState();
	void SetPackage();
	void SetSpectatingPlayersToPlayers();
	UFUNCTION(BlueprintCallable)
	void MakeControllerSpectator(APlayerController* PC);
	
	

	//Create one phase1 GameMode, one phase 2 GameMode and one Phase3 GameMode

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Phase Data")
	float TransitionTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Phase Data")
	EPhaseState State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Phase Data")
	float TimeLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGameActive;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPhase* CurrentPhase;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UPhase*> Phases;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalRounds;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Phase Data")
	FString LevelToLoad;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Phase Data")
	TArray<FString> PossibleLevelsToLoad;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Phase Data")
	FString MainParentLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Phase Data")
	FString StartPhaseName;
	


private:
	
	UPROPERTY()
	float CurrentTime;

	int GetNextPhaseNumber();

	UFUNCTION(BlueprintCallable)
	bool CheckIfGameIsOver();
	bool LoadTransitionStats();
	bool CheckWinnerOfRound();

	void SwitchState();
	void LoadEndLevel();
	void LoadTransitionLevel();
	
	UFUNCTION(BlueprintCallable)
	void AllStagesFinished();
	
};

