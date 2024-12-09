#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "InputActionValue.h" // Required for Enhanced Input
#include "GameFramework/SpringArmComponent.h"
#include "MovementBoat.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RACKETEERS_API UMovementBoat : public UActorComponent
{
	GENERATED_BODY()

public:
	UMovementBoat();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Movement Functions
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Move(FVector2D Value, bool bStarted);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Scurry(bool bIsScurrying);

	// RPCs
	UFUNCTION(Server, Reliable, WithValidation) //Ta bort validation
	void Server_Move(FVector2D Value, bool bStarted);

	UFUNCTION(Server, Reliable)
	void Server_Scurry(bool bIsScurrying);

	UFUNCTION(Client, Reliable)
	void Client_Scurry(bool bIsScurrying);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateTransform(float DeltaTime);

	// Movement Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DecelerationRate;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ScurryAmount;

	// Wave Simulation Properties
	UPROPERTY(EditAnywhere, Category = "Wave Simulation")
	float MovingWaveHeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Wave Simulation")
	float MovingWaveFrequency = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Wave Simulation")
	float PassiveWaveHeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Wave Simulation")
	float PassiveWaveFrequency = 2.0f;

private:
	// Movement Helpers
	void RotateToFaceDirection(const FVector2D& TargetDirection);
	void MoveForward(float DeltaTime, bool bScurryActive);
	void SimulateWaves(float DeltaTime, float WaveHeight, float WaveFrequency);
	FVector GetWorldSpaceDirection(const FVector2D& InputDirection) const;

	// Camera and SpringArm Helpers
	void FindCameraAndSpringArm();
	void Client_InterpolateTransform(float DeltaTime);

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// State Variables
	UPROPERTY()
	float CurrentSpeed = 0.0f;

	UPROPERTY()
	FVector2D MovementInput = FVector2D::ZeroVector;

	UPROPERTY()
	bool bShouldMove = false;

	float DefaultZLocation;

	UPROPERTY()
	bool bScurryIsActive = false;

	UPROPERTY(Replicated)
	FVector ReplicatedLocation;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;

	// Wave Simulation
	float WaveTimeAccumulator = 0.0f;

	// Interpolation
	float InterpolationSpeed = 5.0f;
	float UpdateThreshold = 10.0f; 

	// Predicted Position
	FVector PredictedPosition;

	// References
	UPROPERTY()
	USpringArmComponent* SpringArm;

	UPROPERTY()
	AActor* TeamCamera;

	//UPROPERTY()
	//UPrimitiveComponent* BoatMesh;
};
