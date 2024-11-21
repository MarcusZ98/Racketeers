// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TransitionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginTransition);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACKETEERS_API UTransitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UTransitionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetWidgetWithName(FName Name);
	void AddWidgetsToPlayers(AGameStateBase* GS);
	void RemoveWidgetsFromPlayers();
	UFUNCTION()
	void LoadingFinished();
	UFUNCTION()
	void IncrementPlayerReady();

	UPROPERTY(Blueprintable, BlueprintAssignable, BlueprintCallable)
	FOnFinished OnFinished;
	UPROPERTY(Blueprintable, BlueprintAssignable, BlueprintCallable)
	FOnBeginTransition OnBeginTransition;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame

	UPROPERTY(EditAnywhere, Blueprintable)
	UUserWidget* WidgetToDisplay;
	FName WidgetName;
	bool bIsFinished;
	bool bIsOn;
	int32 CountPlayersReady;
private:

};


