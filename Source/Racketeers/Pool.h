// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledObject.h"
#include "Projectile.h"
#include "Components/ActorComponent.h"
#include "Pool.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACKETEERS_API UPool : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<APooledObject*> ObjectPool;

	UFUNCTION(BlueprintCallable)
	void InitializePool();

	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<AActor> ProjectileClass;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	APooledObject* FindFirstAvailableObject();
	APooledObject* SpawnFromPool(FVector Location, FRotator Rotation);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TSubclassOf<APooledObject> PooledObjectClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	int PoolSize;
		
};
