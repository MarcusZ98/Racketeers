// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeStructs.h"
#include "UObject/Interface.h"
#include "Deposit.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDeposit : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RACKETEERS_API IDeposit
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Deposit")
	void Deposit(AActor* Instigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Deposit")
	void Withdraw(AActor* Instigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Deposit")
	void ShowDepositOutline(AActor* Instigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Deposit")
	void HideDepositOutline(AActor* Instigator);
};
