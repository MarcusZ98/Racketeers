#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponentPool.h"
#include "Projectile.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
//#include "UserWidget.generated.h"
#include "BoatCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(config=Game)
class ABoatCharacter : public ACharacter
{
	GENERATED_BODY()

	/* ---- CAMERA BOOM POSITION ---- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/* ---- FOLLOW CAMERA ---- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/* ---- MAPPING CONTEXT ---- */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/* ---- INPUT ACTIONS ---- */
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
	/* ---- FUNCTIONS ---- */
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Move(const FInputActionValue& Value);
	void Scurry(const FInputActionValue& Value);

	/* ---- RPCs ---- */
	UFUNCTION(Server, Reliable)
	void ServerStartScurry(bool bIsScurrying);
	UFUNCTION(Server, Reliable)
	void ServerStartShooting(bool bLeft);
	UFUNCTION(Server, Reliable)
	void ServerHoldShoot();

	/* ---- ACTORS & COMPONENTS ---- */
	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<AProjectile> ProjectileClass;
	UPROPERTY()
	TArray<USceneComponent*> CannonLeftComponents;
	UPROPERTY()
	TArray<USceneComponent*> CannonRightComponents;
	UPROPERTY()
	TArray<USceneComponent*> CannonComponents;

	/* ---- WIDGETS ---- */
	// Widget Blueprint class to be assigned in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> BoatWidgetClass;
	// Instance of the created widget
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* BoatWidgetInstance;
	
	/* ---- TIMERS ---- */
	FTimerHandle FireTimerHandle;
	FTimerHandle CooldownTimerHandle;

	/* ---- ARRAYS ----*/
	TArray<FName> LeftCannonOrder;
	TArray<FName> RightCannonOrder;
	TArray<FName> CannonChildren;

	FVector SpawnLocation;
	FRotator SpawnRotation;

	/* ---- VARIABLES ----*/
	UPROPERTY(EditAnywhere, Category="Cannons")
	float FireDelay = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cannons")
	float ShootCooldown = 1;
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	bool bIsInteracting;

	// Construction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CannonCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Properties")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat Properties")
	UMaterialInterface* SailMaterial;

	
private:
	/* ---- FUNCTIONS ----*/
	void ShootCannon();
	void ResetShootCooldown();
	void OnShootLeftStarted();
	void OnShootLeftCompleted();
	void OnShootRightStarted();
	void OnShootRightCompleted();
	void AddCannonsInOrder(float Count, USceneComponent* Cannon);
	void ResetScurrySpeed();

	/* ---- VARIABLES ----*/
	float OriginalMaxWalkSpeed; // To store the original speed
	FTimerHandle ScurryTimerHandle; // Timer handle for scurry
	int32 CurrentCannonIndex = 0;
	bool bCanShoot = true;
	bool bShootLeft;
	bool bIsHoldingShoot = false;

	
public:
	/* ---- FUNCTIONS ----*/
	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void StopShooting();
	UFUNCTION(BlueprintCallable)
	void FindCannons();
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	float GetCooldownProgress() const;
	UFUNCTION(BlueprintCallable, Category = "Shoot")
	float GetShootRange() const;

	/* ---- SETTERS ----*/
	/*UFUNCTION(BlueprintCallable, Category = "Boat Properties")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Boat Properties")
	void SetCannonAmount(float NewCannonAmount);

	UFUNCTION(BlueprintCallable, Category = "Boat Properties")
	void SetMovementSpeed(float NewMovementSpeed);

	UFUNCTION(BlueprintCallable, Category = "Boat Properties")
	void SetRotationSpeed(float NewRotationSpeed);

	UFUNCTION(BlueprintCallable, Category = "Boat Properties")
	void SetHullAndSailMaterial(UMaterialInterface* NewHullMaterial, UMaterialInterface* NewSailMaterial);
*/
	/* ---- SOUNDS & PARTICLES ----*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Scurry Effects")
	void PlayScurryEffects();
	UFUNCTION(BlueprintImplementableEvent, Category = "Scurry Effects")
	void StopScurryEffects();
	UFUNCTION(BlueprintImplementableEvent, Category = "Scurry Effects")
	void PlayCannonEffects(USceneComponent* Cannon, UNiagaraComponent* NiagaraComponent);

	/* ---- COMPONENTS ----*/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/* ---- VARIABLES ----*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ScurryAmount;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bScurryActive = false;
	// Replicated remaining cooldown time
	UPROPERTY(Replicated)
	float RemainingCooldownTime;
	UPROPERTY(Replicated)
	float ShootTime;

	/* ---- COMPONENTS ---- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CustomCollisionBox;
};

