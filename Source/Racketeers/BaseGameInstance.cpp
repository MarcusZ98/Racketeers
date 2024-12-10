// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameInstance.h"

#include "Kismet/GameplayStatics.h"

void UBaseGameInstance::SetDataToTransfer_Implementation(FGameStatsPackage GameStatsPackage)
{
	if(TempSavedGameStatsPackage.State == ETransferState::READY)
	{
		UE_LOG(LogTemp, Error, TEXT("The Temp Package has already been filled"));
		return;
	}
	TempSavedGameStatsPackage = GameStatsPackage;
	TempSavedGameStatsPackage.State = ETransferState::READY;
}

bool UBaseGameInstance::SetDataToTransfer_Validate(FGameStatsPackage GameStatsPackage)
{
	return true;
}

bool UBaseGameInstance::CheckIfDataToTransfer()
{
	if(TempSavedGameStatsPackage.State == ETransferState::READY)
		return true;
	return false;
}

FGameStatsPackage UBaseGameInstance::GetDataTransferPackage()
{
	if(TempSavedGameStatsPackage.State == ETransferState::EMPTY)
	{
		UE_LOG(LogTemp, Error, TEXT("You must set stats package first"));
		return FGameStatsPackage();
	}
	TempSavedGameStatsPackage.State = ETransferState::EMPTY;
	return  TempSavedGameStatsPackage;
}


void UBaseGameInstance::SetGameModeData(FGameModeData Data)
{
	TempGameModeData = Data;
}

FGameModeData UBaseGameInstance::GetGameModeData()
{
	FGameModeData Temp = TempGameModeData;
	TempGameModeData = FGameModeData();
	return Temp;
}


void UBaseGameInstance::SetGameStateData_Implementation()
{
	FGameStatsPackage GameStatsPackage = GetDataTransferPackage();
	TempSavedGameStatsPackage =  FGameStatsPackage();

	
}

bool UBaseGameInstance::SetGameStateData_Validate()
{
	return true;
}

void UBaseGameInstance::ClearDataStatsPackage_Implementation()
{
	TempSavedGameStatsPackage = FGameStatsPackage();
}

bool UBaseGameInstance::ClearDataStatsPackage_Validate()
{
	return true;
}
