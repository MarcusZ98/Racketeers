// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledObject.generated.h"

UCLASS()
class RACKETEERS_API APooledObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APooledObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lifetime")
	float TimeToLive = 3;

	FTimerHandle SetInUseTimerHandle;
	
	UFUNCTION()
	void ReturnToPool();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pooling")
	void SetInUse(bool bIsInUse);

	virtual void SetInUse_Implementation(bool bIsInUse);
	
	bool bInUse;
};
