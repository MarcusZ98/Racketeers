// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Lobby.h"
#include "Net/UnrealNetwork.h"

void AGS_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGS_Lobby, bRaccoonFull);
	DOREPLIFETIME(AGS_Lobby, bPandaFull);
	DOREPLIFETIME(AGS_Lobby, bEnoughPlayersToStart);
}
