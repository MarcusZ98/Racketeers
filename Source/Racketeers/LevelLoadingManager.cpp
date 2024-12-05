// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLoadingManager.h"

#include "Phase.h"
#include "RacketeersController.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelLoadingManager::ALevelLoadingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//CurrentLoadedLevel = NewObject<ULevelStreamingDynamic>();
	bReplicates = true;
}



// Called when the game starts or when spawned
void ALevelLoadingManager::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "ALevelLoadingManager::BeginPlay");
	NextLevelPath = "Phase1Parent";
	NextSubLevelPath = "Phase1_GamePlay";
	LoadLevel();

	/*
	CurrentLoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), "Phase1_GamePlay", FVector::ZeroVector,FRotator::ZeroRotator,bLevelLoadSuccessfull);
	if(CurrentLoadedLevel != nullptr){
		CurrentLoadedLevel->OnLevelLoaded.AddDynamic(this, &ALevelLoadingManager::LevelLoaded);
		CurrentLoadedLevel->OnLevelUnloaded.AddDynamic(this, &ALevelLoadingManager::LevelUnloaded);
		GetWorld()->AddStreamingLevel(CurrentLoadedLevel); 
	}
	*/
	
}

// Called every frame
void ALevelLoadingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,FString::FromInt(GetWorld()->IsStreamingLevelBeingConsidered(CurrentLoadedLevel)));

}


void ALevelLoadingManager::MulticastLoadLevel_Implementation(const UPhase* LevelPath)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "LEVEL LOADING MULTICAST LOADING");
	NextLevelPath = LevelPath->MainParentLevel;
	NextSubLevelPath = LevelPath->LevelToLoad;
	UnloadLevel();
	//LoadLevel(LevelPath);
}

void ALevelLoadingManager::LoadLevelParent()
{
	FLatentActionInfo LoadActionInfo;
	
	LoadActionInfo.Linkage = 0;
	LoadActionInfo.CallbackTarget = this;
	LoadActionInfo.ExecutionFunction = TEXT("LoadSubLevel");
	LoadActionInfo.UUID = GetUniqueID();
	UGameplayStatics::LoadStreamLevel(GetWorld(), *CurrentLevelPath, true,false, LoadActionInfo);
}
void ALevelLoadingManager::LoadSubLevel()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "LOAD SUB LEVEL");
	bLevelIsLoading = true;
	OnLoadLevel.Broadcast();
	CurrentLoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), CurrentSubLevelPath, FVector::ZeroVector,FRotator::ZeroRotator,bLevelLoadSuccessfull);

	if(bLevelLoadSuccessfull)
	{
		OnLoadingLevelCompleted.Broadcast();
		bLevelLoadSuccessfull = false;
		bLevelIsLoading = false;
		GetWorld()->AddStreamingLevel(CurrentLoadedLevel);
		return;
	}
	bLevelIsLoading = false;
	OnLoadingLevelCompleted.Broadcast();
}

void ALevelLoadingManager::UnloadLevelParent()
{

	FLatentActionInfo LoadActionInfo;
	
	LoadActionInfo.Linkage = 0;
	LoadActionInfo.CallbackTarget = this;
	LoadActionInfo.ExecutionFunction = TEXT("LoadLevelParent");
	LoadActionInfo.UUID = GetUniqueID();

	UGameplayStatics::UnloadStreamLevel(GetWorld(), *CurrentLevelPath, LoadActionInfo, false);
}


void ALevelLoadingManager::UnloadSubLevel()
{

	
	
}


bool ALevelLoadingManager::MulticastLoadLevel_Validate(const UPhase* LevelPath)
{
	return true;
}


void ALevelLoadingManager::LoadLevel()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "LOAD LEVEL");
	CurrentLevelPath = NextLevelPath;
	CurrentSubLevelPath = NextSubLevelPath;
	LoadLevelParent();
}

void ALevelLoadingManager::UnloadLevel()
{
	FLatentActionInfo LoadActionInfo;
	
	LoadActionInfo.Linkage = 0;
	LoadActionInfo.CallbackTarget = this;
	LoadActionInfo.ExecutionFunction = TEXT("LoadLevel");
	LoadActionInfo.UUID = GetUniqueID();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UNLOAD LEVEL");
	if(CurrentLoadedLevel == nullptr) return;
	OnUnloadLevel.Broadcast();
	CurrentLoadedLevel->SetShouldBeVisible(false);
	CurrentLoadedLevel->SetShouldBeLoaded(false);
	CurrentLoadedLevel->SetIsRequestingUnloadAndRemoval(true);
	//Persistant Level
	//Sub-Levels - Unload the sub-level, 
	//Phase 1
		// Random Phase 1 Map
	//Phase 2
		// Random Phase 2 Map
	//Phase 3
		// Random Phase 3 Map 
	
	GetWorld()->RemoveStreamingLevel(CurrentLoadedLevel);
	UGameplayStatics::UnloadStreamLevel(GetWorld(), *CurrentLevelPath, LoadActionInfo, false);

}

void ALevelLoadingManager::LevelUnloaded()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UNLOAD LEVEL");
	OnUnloadingLevelCompleted.Broadcast();
}

void ALevelLoadingManager::LevelLoaded()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "LOADED LEVEL");
	OnLoadingLevelCompleted.Broadcast();
}
