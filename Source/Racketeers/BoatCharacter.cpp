// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoatCharacter.h"
#include "BoatCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABoatCharacter::ABoatCharacter()
{
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bIsHoldingShoot = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ABoatCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	FindCannons();
}

void ABoatCharacter::Tick(float DeltaTime)
{
	ShootTime += DeltaTime;
}


void ABoatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABoatCharacter::Move);

		// Scurrying
		EnhancedInputComponent->BindAction(ScurryAction, ETriggerEvent::Started, this, &ABoatCharacter::Scurry);

		// Cannon
		EnhancedInputComponent->BindAction(ShootLeftAction, ETriggerEvent::Started, this, &ABoatCharacter::OnShootLeftStarted);
		EnhancedInputComponent->BindAction(ShootLeftAction, ETriggerEvent::Completed, this, &ABoatCharacter::OnShootLeftCompleted);
		EnhancedInputComponent->BindAction(ShootRightAction, ETriggerEvent::Started, this, &ABoatCharacter::OnShootRightStarted);
		EnhancedInputComponent->BindAction(ShootRightAction, ETriggerEvent::Completed, this, &ABoatCharacter::OnShootRightCompleted);
		
	}
}





///// INPUTS /////

void ABoatCharacter::Move(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

		if(CameraManager == nullptr)
		{
			return;
		}

		const FRotator Rotation = CameraManager->GetCameraRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector from camera
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// Get right vector from camera
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}






///// SCURRY /////

void ABoatCharacter::Scurry(const FInputActionValue& Value)
{
	bool bIsScurrying = Value.Get<bool>();
	ServerStartScurry(bIsScurrying);
}

void ABoatCharacter::ServerStartScurry_Implementation(bool bIsScurrying)
{
	if (bIsScurrying && !bScurryActive)
	{
		bScurryActive = true;
		OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed *= ScurryAmount;

		PlayScurryEffects();

		GetWorldTimerManager().SetTimer(ScurryTimerHandle, this, &ABoatCharacter::ResetScurrySpeed, 0.5f, false);
	}
}

void ABoatCharacter::ResetScurrySpeed()
{
	// Reset the speed to the original value
	GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed;

	// Clear the timer
	GetWorldTimerManager().ClearTimer(ScurryTimerHandle);

	// Reset the scurry state
	bScurryActive = false;

	// Stop effects in Blueprint
	StopScurryEffects();
}






///// SHOOT /////

void ABoatCharacter::OnShootLeftStarted()
{
	ServerHoldShoot();
}

void ABoatCharacter::OnShootLeftCompleted()
{
	ServerStartShooting(true);
}

void ABoatCharacter::OnShootRightStarted()
{
	ServerHoldShoot();
}

void ABoatCharacter::OnShootRightCompleted()
{
	ServerStartShooting(false);
}

void ABoatCharacter::ServerHoldShoot_Implementation()
{
	bIsHoldingShoot = true;
	ShootTime = 1;
}

void ABoatCharacter::ServerStartShooting_Implementation(bool bLeft)
{
	if(!bCanShoot || bIsInteracting) return;
	
	bShootLeft = bLeft;
	bIsHoldingShoot = false;
	CurrentCannonIndex = 0;

	// Fire the first cannon immediately
	ShootCannon();

	// Start firing with delay
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ABoatCharacter::ShootCannon, FireDelay, true);

	// Disable further shooting and start the cooldown timer
	bCanShoot = false;
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle, this, &ABoatCharacter::ResetShootCooldown, ShootCooldown, false);
}

void ABoatCharacter::ResetShootCooldown()
{
	bCanShoot = true;
}


void ABoatCharacter::StopShooting()
{
	// Clear the timer to stop firing
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ABoatCharacter::ShootCannon()
{
	CannonComponents = bShootLeft ? CannonLeftComponents : CannonRightComponents;

	if (!CannonComponents.IsValidIndex(CurrentCannonIndex) || !ProjectileClass) return;
	
	USceneComponent* SelectedCannon = CannonComponents[CurrentCannonIndex];
	UNiagaraComponent* SelectedParticle = nullptr;

	if (!SelectedCannon) return;
		
	// Initialize variables for projectile spawn location and rotation
	FVector SpawnLocation = SelectedCannon->GetComponentLocation();
	FRotator SpawnRotation = SelectedCannon->GetComponentRotation();

	// Check child components for specific tags (e.g., "ProjectilePos" and "EffectPos")
	for (int32 i = 0; i < SelectedCannon->GetNumChildrenComponents(); ++i)
	{
		USceneComponent* ChildComponent = Cast<USceneComponent>(SelectedCannon->GetChildComponent(i));
				
		if (ChildComponent)
		{
			if (ChildComponent->ComponentHasTag(FName("ProjectilePos")))
			{
				// Update spawn location and rotation if "ProjectilePos" tag is found
				SpawnLocation = ChildComponent->GetComponentLocation();
				SpawnRotation = ChildComponent->GetComponentRotation();
			}
			else if (ChildComponent->ComponentHasTag(FName("ParticlePos")))
			{
				// Attempt to cast the child component to UNiagaraComponent
				SelectedParticle = Cast<UNiagaraComponent>(ChildComponent);
			}
		}
	}

	// Spawn the projectile at the determined location and rotation
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	 // Cast to your projectile class to call the function
	 Cast<AProjectile>(SpawnedProjectile);
	 if (SpawnedProjectile)
	 {
	 	// Call SetShootRange with the appropriate value
	 	SpawnedProjectile->SetShootRange(ShootTime);
	 }
	
	// Play the cannon effects using the SelectedEffect Niagara component
	PlayCannonEffects(SelectedCannon, SelectedParticle);

	// Increment cannon index for the next shot
	CurrentCannonIndex++;

	// Stop shooting if all cannons have been fired
	if (CurrentCannonIndex >= CannonComponents.Num())
	{
		StopShooting();
	}
}

void ABoatCharacter::FindCannons()
{
	// Clear the array to ensure no duplicates
	CannonLeftComponents.Empty();
	CannonRightComponents.Empty();

	// Find all components with the "Cannon" tag
	TArray<USceneComponent*> FoundComponents;
	GetComponents<USceneComponent>(FoundComponents);

	for (USceneComponent* Component : FoundComponents)
	{
		if (Component->ComponentHasTag(FName("CannonLeft")) && CannonLeftComponents.Num() <= CannonCount)
		{
			CannonLeftComponents.Add(Component);
		}

		else if(Component->ComponentHasTag(FName("CannonRight")) && CannonRightComponents.Num() <= CannonCount)
		{
			CannonRightComponents.Add(Component);
		}
	}
}


	
