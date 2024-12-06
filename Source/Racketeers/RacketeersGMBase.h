// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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




#define MAXTOTALROUNDS 8

UCLASS()
class RACKETEERS_API ARacketeersGMBase : public AGM_Base
{
	GENERATED_BODY()
public:

	ARacketeersGMBase();
	
	//Events / Delegates
	
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


	UFUNCTION(BlueprintCallable)
	void RespawnPlayer(APlayerState* PState);
	UFUNCTION(BlueprintCallable)
	void IncreaseTotalRounds();
	UFUNCTION(BlueprintCallable)
	void DecreaseTotalRounds();
	UFUNCTION(BlueprintCallable)
	void RoundCompletion();

	UFUNCTION(BlueprintCallable)
	void Transition();
	
	void BroadcastOnPlayerPressed(ETeams Team);
	void IncrementPlayerCounter();
	int8 GetTotalRounds();
	TEnumAsByte<EPhaseState> SwitchIncomingState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGameActive;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPhase* CurrentPhase;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UPhase*> Phases;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalRounds;

private:


	UPROPERTY()
	float CurrentTime;

	int GetNextPhaseNumber();
	
	//methods for progressing trough phases

	bool CheckIfGameIsOver();
	bool LoadTransitionStats();
	bool CheckWinnerOfRound();
	bool EndGame();
	void SwitchState();
	


	UFUNCTION(BlueprintCallable)
	void AllStagesFinished();
	
};

