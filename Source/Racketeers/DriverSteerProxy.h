#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoatMovement.h" // Include BoatMovement to access movement functions
#include "DriverSteerProxy.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RACKETEERS_API UDriverSteerProxy : public UActorComponent
{
	GENERATED_BODY()

public:
	UDriverSteerProxy();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void HandleSteerInput(float Value);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerHandleSteerInput(float Value);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	AActor* Boat;

	UPROPERTY(Replicated)
	UBoatMovement* BoatMovementComponent;
};