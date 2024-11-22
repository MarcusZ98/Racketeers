// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "TimerInfo.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RACKETEERS_API ATimerInfo : public AInfo
{
	GENERATED_BODY()
	ATimerInfo();
	void DecreaseTime(float DeltaSeconds);
	public:

	UFUNCTION(BlueprintCallable, Blueprintable)
	static bool GetIsActive(){return ATimerInfo::bIsActive;};
	UFUNCTION(BlueprintCallable, Blueprintable)
	static void SetIsActive(bool active){ATimerInfo::bIsActive = active;};

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 PandasReady;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 RaccoonsReady;
	
	
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION(BlueprintCallable, Blueprintable)
	void SetTimeSeconds(float seconds, bool SetIsActive);
	UFUNCTION(BlueprintCallable, Blueprintable)
	void SetTime_Analog(int32 Minutes, int32 Seconds, bool SetIsActive);
	
	UFUNCTION(BlueprintCallable, Blueprintable)
	int32 GetMinutes();
	UFUNCTION(BlueprintCallable, Blueprintable)
	int32 GetSeconds();
	
	UFUNCTION(Server, reliable)
	void ServerMultiCastActivateTimer();
	
	UFUNCTION(NetMulticast, reliable, BlueprintCallable)
	void MultiCastActivateTimer(float T,  bool SetIsActive);
	
	UFUNCTION(BlueprintCallable, Blueprintable)
	void ActivateTime();
	UFUNCTION(BlueprintCallable, Blueprintable)
	void DeactivateTime();

	UFUNCTION(Blueprintable, BlueprintCallable)
	static float GetTime();
	UFUNCTION(Blueprintable, BlueprintCallable)
	static void SetTime(float T);


private:
	static bool bIsActive;
	static float Time;
};

