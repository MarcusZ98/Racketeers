// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "TimerInfo.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Blueprintable)
class RACKETEERS_API ATimerInfo : public AInfo
{
	GENERATED_BODY()


	ATimerInfo();

	virtual void Tick(float DeltaSeconds) override;

	void DecreaseTime(float DeltaSeconds);
	
	bool bIsActive;
	
	public:

	UFUNCTION(BlueprintCallable, Blueprintable)
	int32 GetMinutes();
	UFUNCTION(BlueprintCallable, Blueprintable)
	int32 GetSeconds();
	UFUNCTION(BlueprintCallable, Blueprintable)
	void ActivateTime();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Time;
	
};