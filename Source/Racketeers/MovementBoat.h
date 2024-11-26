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

	// Function to handle input
	UFUNCTION(BlueprintCallable, Category="Movement")
	void Move(FVector2D Value, bool bStarted);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void Scurry(bool bIsScurrying);

private:
	void RotateToFaceDirection(const FVector2D& InputDirection);
	void MoveForward(float DeltaTime, bool bScurryActive);
	FVector GetWorldSpaceDirection(const FVector2D& InputDirection) const;
	void FindCameraAndSpringArm();

	// Movement properties
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementSpeed = 600.0f;

	float CurrentSpeed = 0.0f; // Current movement speed of the boat

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ScurryAmount = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DecelerationRate = 50.0f; // Rate at which the boat slows down (units per second)

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationSpeed = 5.0f; // Speed of rotation smoothing

	FVector2D MovementInput = FVector2D::ZeroVector;
	
	bool bShouldMove = false;

	bool bScurryIsActive = false;
	
	// Reference to the spring arm within the camera blueprint
	UPROPERTY()
	USpringArmComponent* SpringArm;

	// Reference to the camera blueprint
	UPROPERTY()
	AActor* TeamCamera;
};
