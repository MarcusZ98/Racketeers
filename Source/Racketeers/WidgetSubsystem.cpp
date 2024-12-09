// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"



void UWidgetSubsystem::IncrementPlayersPressed_Implementation()
{
	if(CanRun)
	{
		return;
	}

	
	CountOfPlayersPressed++;
	if(CountOfPlayersPressed == GetWorld()->GetGameState()->PlayerArray.Num())
	{
		TArray<FName> Names;
		ActiveWidgetComponents.GetKeys(Names);
		for (FName Name : Names)
		{
			OnUnload.Broadcast(Name);
		}
		CountOfPlayersPressed = 0;
		SetCanRunTick(true);
	}
}

bool UWidgetSubsystem::IncrementPlayersPressed_Validate()
{
	return true;
}

void UWidgetSubsystem::SetCanRunTick_Implementation(bool b)
{
	CanRun = b;
}

bool UWidgetSubsystem::SetCanRunTick_Validate(bool b)
{
	return true;
}

void UWidgetSubsystem::ActivateWidget(FName Name)

{
	if(!ActiveWidgetComponents.Contains(Name))
	{
		if(GEngine){}
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Could Not Find Widget, Check Name or it dosent exist i map");
	}

	UUserWidget* TempWidget = *MapOfAllPlayerWidget.Find(Name);
	if(TempWidget == nullptr){return;}
	ActiveWidgetComponents.Add(Name, TempWidget);
	TempWidget->AddToViewport();
}

void UWidgetSubsystem::RemoveWidget(FName Name)
{
	if(!ActiveWidgetComponents.Contains(Name)) return;
	UUserWidget* TempWidget = *ActiveWidgetComponents.Find(Name);
	if(TempWidget == nullptr) return;
	ActiveWidgetComponents.Remove(Name);
	TempWidget->RemoveFromParent();
}
