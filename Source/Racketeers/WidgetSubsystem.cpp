// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"

#include "GameFramework/GameStateBase.h"

void UWidgetSubsystem::IncrementPlayersPressed_Implementation()
{
	CountOfPlayersPressed++;
	if(CountOfPlayersPressed == GetWorld()->GetGameState()->PlayerArray.Num())
	{
		TArray<FName> Names;
		WidgetComponents.GetKeys(Names);
		for (FName Name : Names)
		{
			OnUnload.Broadcast(Name);
		}
		CountOfPlayersPressed = 0;
	}
}

bool UWidgetSubsystem::IncrementPlayersPressed_Validate()
{
	return true;
}
