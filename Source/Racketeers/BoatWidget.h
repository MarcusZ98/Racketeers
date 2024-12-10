// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoatWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACKETEERS_API UBoatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Blueprint-implemented event
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UI_PlayShootRange();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UI_PlayAnimationShootRange();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UI_PlayAnimationError();

	// Blueprint-implemented event
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UI_PlayShootCooldown();

	// Blueprint-implemented event
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UI_PlayAnimationShootCooldown();

	// Blueprint-implemented event
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UI_PlayAnimationHealth();
};
