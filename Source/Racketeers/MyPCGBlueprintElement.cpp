// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"
#include "MyPCGBlueprintElement.h"

UMyPCGBlueprintElement::UMyPCGBlueprintElement()
{
	bHasDefaultInPin = false;
	bHasDefaultOutPin = false;
	//Creates a inputPin for pointData//
	FPCGPinProperties PointPin;
	PointPin.Label = TEXT("Points");
	PointPin.AllowedTypes = EPCGDataType::Point;
	PointPin.bAllowMultipleData = true;
	//Creates outputPins for Trees pointData 
	FPCGPinProperties PointOutputPin;
	PointOutputPin.Label = TEXT("TreePoints");
	PointOutputPin.AllowedTypes = EPCGDataType::Point;
	PointOutputPin.bAllowMultipleData = true;
	//Create outputPins for Bushes pointData

	//Adds the custom pins
	CustomInputPins.Add(PointPin);
	CustomOutputPins.Add(PointOutputPin);
}

void UMyPCGBlueprintElement::PostLoad()
{
	Super::PostLoad();
	
}

TArray<FPCGPinProperties> UMyPCGBlueprintElement::GetCustomInputPins() const
{
	TArray<FPCGPinProperties> InputPins;
	FPCGPinProperties PointPin;
	PointPin.Label = TEXT("Points");
	PointPin.AllowedTypes = EPCGDataType::Point;
	PointPin.bAllowMultipleData = true;
	InputPins.Add(PointPin);
	
	return InputPins;
}

void UMyPCGBlueprintElement::ExecuteWithContext_Implementation(FPCGContext& InContext, const FPCGDataCollection& Input,
	FPCGDataCollection& Output)
{
	Super::ExecuteWithContext_Implementation(InContext, Input, Output);
	
	//InPoints = Cast<UPCGPointData>(Input.GetInputsByPin());
	 //Input.GetTaggedTypedInputs<FPCGTaggedData>("Points");
	// // UPCGPointData* PointDataCast;
	// // PointDataCast = Cast<UPCGPointData&>(TaggedData[0]);
	// for (auto Element : Input.GetTaggedTypedInputs<UPCGPointData>("Points"))
	// {
	// 	//InPointData = Cast<UPCGPointData,FPCGTaggedData>(static_cast<const FPCGTaggedData*>(Element.Data));
	// 	//InPointData = Cast<UPCGPointData,FPCGTaggedData>(Element);
	// 	UE_LOG(LogTemp, Log, TEXT("Point Location:"));
	//
	// }
	
}
