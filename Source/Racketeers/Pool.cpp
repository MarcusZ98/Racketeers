// Fill out your copyright notice in the Description page of Project Settings.


#include "Pool.h"

// Sets default values for this component's properties
UPool::UPool()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPool::BeginPlay()
{
	Super::BeginPlay();
	PoolSize = 10;

	//InitializePool();

	// ...
	
}


// Called every frame
void UPool::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPool::InitializePool()
{
	// Define the spawn location and rotation
	// Define the transform
	FTransform SpawnTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	
	for (int i = 0; i < PoolSize; ++i)
	{
		APooledObject* SpawnedPooledObject = GetWorld()->SpawnActorDeferred<APooledObject>(PooledObjectClass, SpawnTransform, nullptr, nullptr);

		if(SpawnedPooledObject)
		{
			ObjectPool.AddUnique(SpawnedPooledObject);
			SpawnedPooledObject->SetInUse(false);
		}
	}
	
}

APooledObject* UPool::FindFirstAvailableObject()
{
	for (APooledObject* PooledObject : ObjectPool)
	{
		if(PooledObject && !PooledObject->bInUse)
		{
			return PooledObject;
		}
	}
	return nullptr;
}

APooledObject* UPool::SpawnFromPool(FVector Location, FRotator Rotation)
{
	APooledObject* AvailableObject = FindFirstAvailableObject();

	if(IsValid(AvailableObject))
	{
		AvailableObject->SetActorLocation(Location);
		AvailableObject->SetActorRotation(Rotation);
		AvailableObject->SetInUse(true);

		return AvailableObject;
	} else
	{
		// Failed to spawn object
		return nullptr;
	}
}




