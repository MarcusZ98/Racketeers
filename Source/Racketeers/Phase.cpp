// Fill out your copyright notice in the Description page of Project Settings.


#include "Phase.h"

#include "RacketeersGameStateBase.h"
#include "RacketeersGMBase.h"
#include "Kismet/GameplayStatics.h"

UPhase::UPhase()
{
	PrimaryComponentTick.bCanEverTick = false;
	TimeLimit = 0;
	State = EPhaseState::Phase_1;
}


void UPhase::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPhase::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

