// Fill out your copyright notice in the Description page of Project Settings.


#include "PooledObject.h"

// Sets default values
APooledObject::APooledObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APooledObject::BeginPlay()
{
	Super::BeginPlay();

	SetInUse(false);
}

// Called every frame
void APooledObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APooledObject::SetInUse_Implementation(bool bIsInUse)
{
	bInUse = bIsInUse;

	SetActorEnableCollision(bIsInUse);
	SetActorHiddenInGame(!bIsInUse);
	SetActorTickEnabled(bIsInUse);

	// Clear the timer
	GetWorldTimerManager().ClearTimer(SetInUseTimerHandle);

	if(bIsInUse)
	{
		GetWorldTimerManager().SetTimer(SetInUseTimerHandle, this, &APooledObject::ReturnToPool, TimeToLive, false);
	}
}

void APooledObject::ReturnToPool()
{
	SetInUse(false);
}



