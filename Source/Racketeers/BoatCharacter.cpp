// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoatCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ABoatCharacter::ABoatCharacter()
{
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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
	
}

void ABoatCharacter::OnShootLeftCompleted()
{
	bShootLeft = true;
	ServerStartShooting(true);
}

void ABoatCharacter::OnShootRightStarted()
{
	
}

void ABoatCharacter::OnShootRightCompleted()
{
	bShootLeft = false;
	ServerStartShooting(false);
}

void ABoatCharacter::ServerStartShooting_Implementation(bool bLeft)
{
	bShootLeft = bLeft;
	CurrentCannonIndex = 0;

	// Fire the first cannon immediately
	ShootCannon();

	// Start firing with delay
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ABoatCharacter::ShootCannon, FireDelay, true);
}

void ABoatCharacter::StopShooting()
{
	// Clear the timer to stop firing
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ABoatCharacter::ShootCannon()
{
	if (bShootLeft)
	{
		CannonComponents = CannonLeftComponents;
	}
	else
	{
		CannonComponents = CannonRightComponents;
	}

	if (CannonComponents.IsValidIndex(CurrentCannonIndex) && ProjectileClass)
	{
		USceneComponent* SelectedCannon = CannonComponents[CurrentCannonIndex];

		if (SelectedCannon)
		{
			// Initialize variables for projectile spawn location and rotation
			FVector SpawnLocation = SelectedCannon->GetComponentLocation();
			FRotator SpawnRotation = SelectedCannon->GetComponentRotation();

			// Check child components for a specific tag (e.g., "Projectile")
			for (int32 ChildIndex = 0; ChildIndex < SelectedCannon->GetNumChildrenComponents(); ++ChildIndex)
			{
				USceneComponent* ChildComponent = Cast<USceneComponent>(SelectedCannon->GetChildComponent(ChildIndex));
				if (ChildComponent && ChildComponent->ComponentHasTag(FName("ProjectilePos")))
				{
					SpawnLocation = ChildComponent->GetComponentLocation();
					SpawnRotation = ChildComponent->GetComponentRotation();
					break; // Found the projectile component, no need to check further
				} 
			}

			// Spawn the projectile at the determined location and rotation
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			GetWorld()->SpawnActor<AActor>(
				ProjectileClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

			//PlayCannonEffects(SelectedCannon, SelectedEffect);
		}
	}

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

	
