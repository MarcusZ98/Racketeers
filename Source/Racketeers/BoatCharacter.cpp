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
#include "Components/ProgressBar.h"
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

	// Initialize replication
	bReplicates = true;
	//bReplicateMovement = true;
	GetCharacterMovement()->SetIsReplicated(true);
	NetUpdateFrequency = 120.0f;
	MinNetUpdateFrequency = 60.0f;

}

void ABoatCharacter::BeginPlay()
{
	SetReplicateMovement(true);
	// Call the base class  
	Super::BeginPlay();
}

void ABoatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (BoatWidgetInstance)
	{
		BoatWidget = Cast<UBoatWidget>(BoatWidgetInstance);
		if (BoatWidget && IsLocallyControlled())
		{
			ShootTime += DeltaTime;
			if (bIsHoldingShoot && !bIsShooting && bCanShoot && ShootTime < 3)
			{
				BoatWidget->UI_PlayShootRange(); // Play charge animation while holding
			}

			if (!bCanShoot)
			{
				// Play the shoot range animation once the button is released
				BoatWidget->UI_PlayAnimationShootRange();
			}
		}
	}

	if (HasAuthority()) // Only update on the server
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle))
		{
			// Update remaining cooldown time for replication
			RemainingCooldownTime = GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
		}
		else
		{
			RemainingCooldownTime = 0.0f; // Cooldown is complete
		}
	}
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
	bIsHoldingShoot = true;
	ServerHoldShoot();
}

void ABoatCharacter::OnShootLeftCompleted()
{
	bIsHoldingShoot = false;
	ServerStartShooting(true);
}

void ABoatCharacter::OnShootRightStarted()
{
	bIsHoldingShoot = true;
	ServerHoldShoot();
}

void ABoatCharacter::OnShootRightCompleted()
{
	bIsHoldingShoot = false;
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
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABoatCharacter::ResetShootCooldown, ShootCooldown, false);
}

void ABoatCharacter::ResetShootCooldown()
{
	bCanShoot = true;
}


void ABoatCharacter::StopShooting()
{
	bIsShooting = false;
	// Clear the timer to stop firing
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ABoatCharacter::ShootCannon()
{
	// Use pre-ordered CannonComponents
	CannonComponents = bShootLeft ? CannonLeftComponents : CannonRightComponents;

	if (!CannonComponents.IsValidIndex(CurrentCannonIndex) || !ProjectileClass) return;

	USceneComponent* SelectedCannon = CannonComponents[CurrentCannonIndex];
	UNiagaraComponent* SelectedParticle = nullptr;

	if (!SelectedCannon) return;

	// Iterate through child components to locate ProjectilePos and ParticlePos
	for (int32 i = 0; i < SelectedCannon->GetNumChildrenComponents(); ++i)
	{
		USceneComponent* ChildComponent = Cast<USceneComponent>(SelectedCannon->GetChildComponent(i));
		if (ChildComponent)
		{
			if (ChildComponent->ComponentHasTag(FName("ProjectilePos")))
			{
				SpawnLocation = ChildComponent->GetComponentLocation();
				SpawnRotation = ChildComponent->GetComponentRotation();
			}
			else if (ChildComponent->ComponentHasTag(FName("ParticlePos")))
			{
				SelectedParticle = Cast<UNiagaraComponent>(ChildComponent);
			}
		}
	}

	// Spawn the projectile
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (SpawnedProjectile)
	{
		SpawnedProjectile->SetShootRange(ShootTime);
	}

	// Play the cannon effects
	PlayCannonEffects(SelectedCannon, SelectedParticle);

	// Increment the cannon index
	CurrentCannonIndex++;
	bIsShooting = true;

	// Stop shooting if all cannons have been fired
	if (CurrentCannonIndex >= CannonComponents.Num())
	{
		StopShooting();
	}
}

void ABoatCharacter::FindCannons()
{
	CannonLeftComponents.Empty();
	CannonRightComponents.Empty();

	// Gather all scene components
	TArray<USceneComponent*> FoundComponents;
	GetComponents<USceneComponent>(FoundComponents);

	// Add cannons in order based on predefined logic
	for (USceneComponent* Cannon : FoundComponents)
	{
		if (Cannon)
		{
			AddCannonsInOrder(CannonCount, Cannon);
		}
	}

}

void ABoatCharacter::AddCannonsInOrder(float Count, USceneComponent* Cannon)
{
	switch (CannonCount)
	{
	case 1:
		LeftCannonOrder = {FName("CannonL1")};
		RightCannonOrder = {FName("CannonR1")};
		break;
		
	case 2:
		LeftCannonOrder = {FName("CannonL2"), FName("CannonL3")};
		RightCannonOrder = {FName("CannonR3"), FName("CannonR2")};
		break;
		
	case 3:
		LeftCannonOrder = {FName("CannonL2"), FName("CannonL1"), FName("CannonL3")};
		RightCannonOrder = {FName("CannonR2"), FName("CannonR1"), FName("CannonR3")};
		break;

	default:
		break;
		
	}
	
	// Add to LeftCannonComponents if matches LeftCannonOrder
	for (const FName& Tag : LeftCannonOrder)
	{
		if (Cannon->ComponentHasTag(Tag))
		{
			CannonLeftComponents.Add(Cannon);
			Cannon->SetVisibility(true);
			return; // Ensure each cannon is added only once
		}
	}

	// Add to RightCannonComponents if matches RightCannonOrder
	for (const FName& Tag : RightCannonOrder)
	{
		if (Cannon->ComponentHasTag(Tag))
		{
			CannonRightComponents.Add(Cannon);
			Cannon->SetVisibility(true);
			return; // Ensure each cannon is added only once
		}
	}
}

float ABoatCharacter::GetCooldownProgress() const
{
	// Calculate progress based on RemainingCooldownTime
	if (ShootCooldown > 0.0f)
	{
		return 1.0f - FMath::Clamp(RemainingCooldownTime / ShootCooldown, 0.0f, 1.0f);
	}

	// Default to fully charged if cooldown is invalid
	return 1.0f;
}

float ABoatCharacter::GetShootRange() const
{
	return ShootTime;
}

void ABoatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate remaining cooldown time
	DOREPLIFETIME(ABoatCharacter, RemainingCooldownTime);
	DOREPLIFETIME(ABoatCharacter, ShootTime);
	DOREPLIFETIME(ABoatCharacter, bIsHoldingShoot);
	DOREPLIFETIME(ABoatCharacter, bCanShoot);
	DOREPLIFETIME(ABoatCharacter, bIsShooting);

}



/* ---- SETTERS ---- */
/*void ABoatCharacter::SetHealth(float NewHealth)
{
	//Health =  // Assuming a max health value of 1000
}

// Set Cannon Amount
void ABoatCharacter::SetCannonAmount(float NewCannonAmount)
{
	CannonCount = NewCannonAmount;
}

// Set Movement Speed
void ABoatCharacter::SetMovementSpeed(float NewMovementSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

// Set Rotation Speed
void ABoatCharacter::SetRotationSpeed(float NewRotationSpeed)
{
	//RotationSpeed = NewRotationSpeed;
}

// Set Sail Material
void ABoatCharacter::SetHullAndSailMaterial(UMaterialInterface* NewHullMaterial, UMaterialInterface* NewSailMaterial)
{
	if (!NewSailMaterial || !NewHullMaterial) return;

	// Get all StaticMeshComponents in this actor
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		// Check if the mesh has a specific tag or name that identifies it as a sail
		if (MeshComponent->ComponentHasTag(FName("HullAndSail")))
		{
			MeshComponent->SetMaterial(0, NewHullMaterial);
			MeshComponent->SetMaterial(1,  SailMaterial);
		} 
	}
}
*/




	
