// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerInfo.h"

#include "Net/UnrealNetwork.h"

float ATimerInfo::Time;
bool ATimerInfo::bIsActive;

ATimerInfo::ATimerInfo()
{
	//bIsActive = false;
	bReplicates = true;
;
	UE_LOG(LogTemp, Warning, TEXT("TimerInfo::ATimerInfo()"));
}


void ATimerInfo::Tick(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("TimerInfo::Tick()"));
	if(GEngine)
	{
		FString TeamName = "TimerInfo::Tick()";
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *TeamName);
	}
	Super::Tick(DeltaSeconds);
	if(bIsActive)
	{

		DecreaseTime(DeltaSeconds);
	}
}


void ATimerInfo::DecreaseTime(float DeltaSeconds)
{
	Time -= DeltaSeconds;
	if(Time <= 0)
	{
	  Time = 0;
	}
}

void ATimerInfo::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ATimerInfo, bIsActive);
}

void ATimerInfo::SetTimeSeconds(float seconds, bool SetIsActive)
{
		ATimerInfo::SetIsActive(SetIsActive);
		ATimerInfo::SetTime(seconds);
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "TimerInfo::SetTimeSeconds");
}

void ATimerInfo::SetTime_Analog(int32 Minutes, int32 Seconds, bool SetIsActive)
{
	Time = Minutes * 60;
	Time += Seconds;
	bIsActive = SetIsActive;
}

void ATimerInfo::ActivateTime()
{
	bIsActive = true;
}

void ATimerInfo::DeactivateTime()
{
	bIsActive = false;
}

float ATimerInfo::GetTime()
{
	return Time;
}

void ATimerInfo::SetTime(float T)
{
	Time = T;
}
void ATimerInfo::ServerMultiCastActivateTimer_Implementation()
{
	MultiCastActivateTimer(GetTime(), GetIsActive());
	FString string = "SERVER MULTI CAST, " + FString::SanitizeFloat(GetTime()) + " Bool,";
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *string);
}

void ATimerInfo::MultiCastActivateTimer_Implementation(float T, bool SetIsActive)
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "MultiCastActivateTimer_Implementation");
	SetTimeSeconds(T, SetIsActive);
}

int32 ATimerInfo::GetMinutes()
{
	return (int32)(Time/60);
}

int32 ATimerInfo::GetSeconds()
{
	
	return (int32)Time % 60 ;
}
