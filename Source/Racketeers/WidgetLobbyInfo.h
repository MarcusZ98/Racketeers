// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PS_Lobby.h"
#include "Blueprint/UserWidget.h"
#include "WidgetLobbyInfo.generated.h"

/**
 * 
 */
UCLASS()
class RACKETEERS_API UWidgetLobbyInfo : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LobbyInfo")
	FLobbyInfo LobbyInfo;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "LobbyInfo")
	void UpdateLobbyInfo(const FLobbyInfo& NewLobbyInfo, APlayerState* PS);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "LobbyInfo")
	void SetReadyStatus(bool bIsReady);
};
