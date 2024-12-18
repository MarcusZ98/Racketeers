
#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "Phase.Generated.h"

/*
	 *	Fases class
	 *
	 *		problems
	 *
	 *		When respawning  when has been in a boat
	 *		boat part of level so need to remove before unloading or, make the parts presistant and just do it when the player respawns
	 *
	 *
	 *		OnUnloadingMap
	 *
	 *	Phase 1
	 *		- Next Face condition is a time limit.
	 *	Phase 2
	 *		- Next Face Condition is a time limit.
	 *	Phase 3
	 *		- Determinig end of face is a timer
	 *		- lose condition is when the teams ship is destroyed or the team has less health than the other team
	 *		- win condition is when the other teams ship is destroyed or the team has more health than the other
	 *
	 *
	 *
	 *		
	 *
	 */

UENUM(BlueprintType)
enum class EPhaseState : uint8
{
	Phase_1 = 0,
	Phase_2 = 1,
	Phase_3 = 2,
	Phase_NONE = 3
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class RACKETEERS_API UPhase : public UActorComponent
{
public:
	GENERATED_BODY()
	 UPhase();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	float TimeLimit;
	UPROPERTY(BlueprintReadWrite, Category= "Phase Data")
	EPhaseState State;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Phase Data")
	FString LevelToLoad;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Phase Data")
	FString MainParentLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Phase Data")
	FString StartPhaseName;

	//TArray<FTransform> SpawnsTeamA;
	//TArray<FTransform> SpawnsTeamB;
	
	//PlayerSpawns


	
	
};
