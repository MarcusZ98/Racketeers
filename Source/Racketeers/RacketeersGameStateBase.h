// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "GS_Base.h"
#include "Phase.h"
#include "PS_Base.h"
#include "RacketeersGameStateBase.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseOneActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseTwoActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseThreeActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIncomingPhaseOneActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIncomingPhaseTwoActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIncomingPhaseThreeActive);


UCLASS()
class RACKETEERS_API ARacketeersGameStateBase : public AGS_Base
{
	GENERATED_BODY()

	public:

	ARacketeersGameStateBase();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPhaseOneActive OnPhaseOneActive;
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPhaseTwoActive OnPhaseTwoActive;
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPhaseThreeActive OnPhaseThreeActive;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnIncomingPhaseOneActive OnIncomingPhaseOneActive;
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnIncomingPhaseTwoActive OnIncomingPhaseTwoActive;
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnIncomingPhaseThreeActive OnIncomingPhaseThreeActive;

	void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ChangeCurrentPhase(TEnumAsByte<EPhaseState> NewPhase);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetMaxHealth(ETeams Team, int32 MaxHealth);
	UFUNCTION(BlueprintCallable)
	void DamageBoat(int Amount, ETeams Team);
	UFUNCTION(BlueprintCallable)
	void RequestToRemoveWidget();
	UFUNCTION(BlueprintCallable)
	void OnRep_PickUp();
	UFUNCTION(BlueprintCallable)
	void OnRep_PhaseChange();
	UFUNCTION(BlueprintCallable)
	void OnRep_IncomingPhaseChange();
	UFUNCTION(BlueprintCallable)
	void OnRep_HealthChanged();
	UFUNCTION(BlueprintCallable)
	void SetRandomNumber(int Number);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void AddResource(int Amount, EResources Resource, ETeams Team);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RemoveResource(int Amount, EResources Resource, ETeams Team);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void AddToStats(int Amount, EGameStats Stat, ETeams Team);
	UFUNCTION(BlueprintCallable)
	int32 GetTeamResources(ETeams Team, EResources Resource) const;
	UFUNCTION(BlueprintCallable)
	FTeamGameStats GetTeamStats(ETeams Team);
	UFUNCTION(BlueprintCallable)
	virtual void UpdateTeamAlive();
	UFUNCTION(BlueprintCallable)
	void UpdateHealth();
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AddPart(ETeams Team, EPartSpacing Part, EPart NewPart);
	UFUNCTION(NetMulticast, Reliable)
	void RemovePart(ETeams Team, EPart Part);

	bool CheckTeamAlive(ETeams Team);
	void CheckRoundEnd(ETeams Team);

	UPROPERTY(ReplicatedUsing=OnRep_PickUp, BlueprintReadWrite)
	FResources RacconResource;
	UPROPERTY(ReplicatedUsing=OnRep_PickUp, BlueprintReadWrite)
	FResources RedPandasResource;
	UPROPERTY(ReplicatedUsing=OnRep_PhaseChange, BlueprintReadWrite)
	TEnumAsByte<EPhaseState> CurrentPhase;
	UPROPERTY(ReplicatedUsing=OnRep_IncomingPhaseChange, BlueprintReadWrite)
	TEnumAsByte<EPhaseState> IncomingPhase;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FTeamGameStats RaccoonsGameStats;
	UPROPERTY(Replicated, BlueprintReadWrite)
	FTeamGameStats RedPandasGameStats;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FGameStats TeamStats;
	UPROPERTY(BlueprintReadWrite)
	int ExpectedPlayers;


	UPROPERTY(Replicated, BlueprintReadWrite)
	FTeamShipParts RaccoonParts;
	UPROPERTY(Replicated, BlueprintReadWrite)
	FTeamShipParts PandasParts;


	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 RacconsRoundsWon;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	float RaccoonsMaxHealth;
	UPROPERTY(ReplicatedUsing=OnRep_HealthChanged, BlueprintReadWrite)
	float RaccoonsBoatHealth;  // - repnotify
	

	UPROPERTY(ReplicatedUsing=OnRep_HealthChanged, BlueprintReadWrite)
	int32 RedPandasRoundsWon;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	float RedPandasMaxHealth;
	UPROPERTY(ReplicatedUsing=OnRep_HealthChanged, BlueprintReadWrite)
	float RedPandasBoatHealth; // - repnotify
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	ETeams GameWinner = ETeams::NONE;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	float Phase2RandomNumber; 


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 PandasReady;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 RaccoonsReady;

	
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 Phase3RandomNumber;
private:
	void CheckOnRepHealthChanged();
};

