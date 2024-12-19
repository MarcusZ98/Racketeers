// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elements/PCGExecuteBlueprint.h"
#include "MyPCGBlueprintElement.generated.h"

/**
 * 
 */
UCLASS()
class RACKETEERS_API UMyPCGBlueprintElement : public UPCGBlueprintElement
{
	GENERATED_BODY()

public:
	// Constructor
	UMyPCGBlueprintElement();
	// Override function to disable default pins
	virtual void PostLoad() override;

	/**
	 * 
	 */
	UPCGPointData* InPointData;
	TArray<FPCGPoint> InPoints;

protected:
	//Override to define custom input pins

	virtual TArray<FPCGPinProperties> GetCustomInputPins() const ;
	virtual void ExecuteWithContext_Implementation(FPCGContext& InContext, const FPCGDataCollection& Input, FPCGDataCollection& Output) override;
};
