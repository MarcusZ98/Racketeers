// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ULevelSubSystem.generated.h"

class ULevelStreamingDynamic;




DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadingFinished);

UCLASS()
class RACKETEERS_API UULevelSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnLoadingFinished OnLoadingFinished;
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnUnloadingFinished OnUnloadingFinished;
	
	void Run();
	
private:
	FString CurrentLevelName;
	UPROPERTY()
	ULevelStreamingDynamic* CurrentLevel;
	
	void LoadLevel();
	void UnloadLevel();
	void ServerLoadLevel();
	void ServerUnloadLevel();
	void LoadOnAllClients();
	void UnloadOnAllClients();
	
};
