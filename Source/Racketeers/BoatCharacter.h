#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "BoatCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

//DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABoatCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ScurryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootRightAction;

	

public:
	ABoatCharacter();
	

protected:
	// Scurry
	UFUNCTION(Server, Reliable)
	void ServerStartScurry(bool bIsScurrying);

	//UFUNCTION(Server, Reliable)
	//void ServerStopScurry();
	
	// // Shooting
	// UFUNCTION(Server, Reliable)
	// void ServerStartShooting();

	//UFUNCTION(Server, Reliable)
	//void ServerStopShooting();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void Scurry(const FInputActionValue& Value);
	void ResetScurrySpeed();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	/*// Cannon components (assigned in Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons")
	TArray<USceneComponent*> CannonLeftComponents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons")
	TArray<USceneComponent*> CannonRightComponents;
	UPROPERTY()
	TArray<USceneComponent*> CannonComponents;

	// Projectile class
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;

	// Timer for sequential shots
	FTimerHandle FireTimerHandle;

	// Cannon shoot delay
	UPROPERTY(EditDefaultsOnly, Category = "Cannons")
	float FireDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannons")
	int32 CannonCount = 0; // Default value
	int32 CurrentCannonIndex = 0;*/

private:
	float OriginalMaxWalkSpeed; // To store the original speed
	FTimerHandle ScurryTimerHandle; // Timer handle for scurry

	// void ShootCannon();
	// void OnShootLeftStarted();
	// void OnShootLeftCompleted();
	// void OnShootRightStarted();
	// void OnShootRightCompleted();
	// void FindCannons();

public:
	// UFUNCTION(BlueprintCallable, Category = "Shooting")
	// void StartShooting();
	//
	// UFUNCTION(BlueprintCallable, Category = "Shooting")
	// void StopShooting();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Scurry Effects")
	void PlayScurryEffects();

	UFUNCTION(BlueprintImplementableEvent, Category = "Scurry Effects")
	void StopScurryEffects();

	// UFUNCTION(BlueprintImplementableEvent, Category = "Scurry Effects")
	// void PlayCannonEffects(USceneComponent* Cannon);
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Scurry Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ScurryAmount;

	// UPROPERTY(Replicated)
	// bool bShootLeft;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bScurryActive = false; // Tracks whether scurry is currently active

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CustomCollisionBox;
};

