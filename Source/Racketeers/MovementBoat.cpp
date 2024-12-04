#include "MovementBoat.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameStateBase.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "InputActionValue.h"
#include "PS_Base.h"
#include "Camera/CameraComponent.h"

UMovementBoat::UMovementBoat()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);

    bShouldMove = false;
    bScurryIsActive = false;
    CurrentSpeed = 0.0f;
}

void UMovementBoat::BeginPlay()
{
    Super::BeginPlay();
    //FindCameraAndSpringArm();

    // Get BoatMesh for all transform updates
    BoatMesh = Cast<UPrimitiveComponent>(GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass()));
    if (!BoatMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("BoatMesh not found!"));
        return;
    }
    
    // Store the default Z location
    DefaultZLocation = BoatMesh->GetComponentLocation().Z;

    // Initialize replication variables
    ReplicatedLocation = BoatMesh->GetComponentLocation();
    ReplicatedRotation = BoatMesh->GetComponentRotation();
}

void UMovementBoat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (GetOwner()->HasAuthority()) // Server side
    {
        if (bShouldMove)
        {
            MoveForward(DeltaTime, bScurryIsActive);
            RotateToFaceDirection(MovementInput);
            SimulateWaves(DeltaTime, MovingWaveHeight, MovingWaveFrequency);
        }else if(!bShouldMove)
        {
            SimulateWaves(DeltaTime, PassiveWaveHeight, PassiveWaveFrequency);
        }

        Server_UpdateTransform(DeltaTime); // Send transform updates to clients
    }
    else if(!GetOwner()->HasAuthority() && bShouldMove)// Client side
    {
        Client_InterpolateTransform(DeltaTime); // Smooth client-side interpolation
    }
}

void UMovementBoat::Move(FVector2D Value, bool bStarted)
{
    //FindCameraAndSpringArm();
    bShouldMove = bStarted;

    if (bStarted)
    {
        MovementInput = Value.GetSafeNormal();
        CurrentSpeed = MovementSpeed;
    }

    if (GetOwnerRole() < ROLE_Authority) // Client sends input to the server
    {
        Server_Move(Value, bStarted);
    }
}

void UMovementBoat::Server_Move_Implementation(FVector2D Value, bool bStarted)
{
    Move(Value, bStarted);
}

bool UMovementBoat::Server_Move_Validate(FVector2D Value, bool bStarted)
{
    return true;
}

void UMovementBoat::Scurry(bool bIsScurrying)
{
    // Apply scurry effect locally on the client immediately
    if (GetOwnerRole() == ROLE_AutonomousProxy)
    {
        bScurryIsActive = bIsScurrying;

        // Send the request to the server to track the change
        Server_Scurry(bIsScurrying);
    }
    
    // On the server side, handle scurry state for the controlling player
    if (GetOwner()->HasAuthority())
    {
        bScurryIsActive = bIsScurrying;
    }
}

void UMovementBoat::Server_Scurry_Implementation(bool bIsScurrying)
{
    // Server processes scurry but does not apply it globally.
    bScurryIsActive = bIsScurrying;
}

void UMovementBoat::Client_Scurry_Implementation(bool bIsScurrying)
{
    // This function is executed locally to ensure responsiveness
    bScurryIsActive = bIsScurrying;
}


void UMovementBoat::RotateToFaceDirection(const FVector2D& InputDirection)
{
    if (InputDirection.IsNearlyZero())
    {
        return; // No input, no rotation
    }

    // Convert the 2D input direction into a world-space direction
    FVector TargetDirection = GetWorldSpaceDirection(InputDirection).GetSafeNormal();

    if (!TargetDirection.IsNearlyZero())
    {
        FRotator CurrentRotation = BoatMesh->GetComponentRotation();
        FRotator TargetRotation = TargetDirection.Rotation();

        TargetRotation.Pitch = 0.0f; // Ensure no pitch
        TargetRotation.Roll = 0.0f; // Ensure no roll

        // Smoothly interpolate to the target rotation
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
        BoatMesh->SetWorldRotation(NewRotation, true);
    }
}

void UMovementBoat::MoveForward(float DeltaTime, bool bScurryActive)
{
    if (MovementInput.IsNearlyZero())
    {
        // Reset boat location.z to make z position consistent
        // Get the current location
        FVector CurrentLocation = BoatMesh->GetComponentLocation();

        // Reset only the Z component
        CurrentLocation.Z = DefaultZLocation;

        // Set the new location
        BoatMesh->SetWorldLocation(CurrentLocation, true);
        
        return; // No input, do nothing
    }

    FVector DesiredDirection = GetWorldSpaceDirection(MovementInput).GetSafeNormal();

    if (!DesiredDirection.IsNearlyZero())
    {
        float CurrentScurryMultiplier = bScurryActive ? ScurryAmount : 1.0f;

        FVector CurrentLocation = BoatMesh->GetComponentLocation();
        FVector NewLocation = CurrentLocation + (DesiredDirection * CurrentSpeed * CurrentScurryMultiplier * DeltaTime);

        // Allow Z movement as intended
        BoatMesh->SetWorldLocation(NewLocation, true);

        // Update replication variables
        if (GetOwnerRole() == ROLE_Authority)
        {
            ReplicatedLocation = NewLocation;
            ReplicatedRotation = BoatMesh->GetComponentRotation();
        }
    }
}

FVector UMovementBoat::GetWorldSpaceDirection(const FVector2D& InputDirection) const
{
    /*// Get the player camera's rotation
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return FVector::ZeroVector;

    FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
    FRotator YawRotation(0.0f, CameraRotation.Yaw, 0.0f);

    // Use camera's yaw rotation to calculate forward and right vectors
    FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // Transform input direction into world space
    FVector WorldSpaceDirection = (ForwardVector * InputDirection.Y + RightVector * InputDirection.X).GetSafeNormal();
    return WorldSpaceDirection;*/

    // Find the boat's camera component
    
    UCameraComponent* BoatCamera = GetOwner()->FindComponentByClass<UCameraComponent>();
    if (!BoatCamera)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boat camera not found! Falling back to actor's forward vector."));
        return FVector::ZeroVector;
    }

    // Get the camera's rotation
    FRotator CameraRotation = BoatCamera->GetComponentRotation();
    FRotator YawRotation(0.0f, CameraRotation.Yaw, 0.0f); // Ignore pitch and roll for movement

    // Calculate forward and right vectors based on the camera's yaw
    FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // Combine input directions (X = forward/backward, Y = left/right)
    FVector WorldSpaceDirection = (ForwardVector * InputDirection.Y + RightVector * InputDirection.X).GetSafeNormal();
    return WorldSpaceDirection;
}

void UMovementBoat::SimulateWaves(float DeltaTime, float WaveHeight, float WaveFrequency)
{

    FVector CurrentLocation = BoatMesh->GetComponentLocation();

    // Update the wave time accumulator
    WaveTimeAccumulator += DeltaTime * WaveFrequency;

    // Calculate the wave offset
    float WaveOffset = FMath::Sin(WaveTimeAccumulator) * WaveHeight;

    // Apply the wave offset to the Z location
    CurrentLocation.Z += WaveOffset;

    // Set the new location
    BoatMesh->SetWorldLocation(CurrentLocation, true);
}


void UMovementBoat::FindCameraAndSpringArm()
{
    TeamCamera = nullptr;
    SpringArm = nullptr;

    //FName CameraTag;
    //if (GetOwner()->ActorHasTag("BoatPanda"))
    //{
    //    CameraTag = FName("CameraPanda");
    //}
   // else if (GetOwner()->ActorHasTag("BoatRaccoon"))
   // {
   //     CameraTag = FName("CameraRaccoon");
  // }
   // else
   // {
   //     UE_LOG(LogTemp, Error, TEXT("Boat does not have a valid team tag!"));
    //    return;
   // }

   // for (TActorIterator<AActor> It(GetWorld()); It; ++It)
   // {
     //   if (It->ActorHasTag(CameraTag))
    //    {
    //        TeamCamera = *It;
      //      return;
     //   }
   // }
    SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
}

void UMovementBoat::Client_InterpolateTransform(float DeltaTime)
{
    if (!BoatMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("BoatMesh is null in Client_InterpolateTransform!"));
        return;
    }

    // Accumulate time since last interpolation
    static float TimeSinceLastUpdate = 0.0f;
    TimeSinceLastUpdate += DeltaTime;

    // Define a lerp alpha to control smoothing
    const float LerpSpeed = 5.0f; // Adjust for smoother transitions
    float Alpha = FMath::Clamp(TimeSinceLastUpdate * LerpSpeed, 0.0f, 1.0f);

    // Current position and rotation
    FVector CurrentLocation = BoatMesh->GetComponentLocation();
    FRotator CurrentRotation = BoatMesh->GetComponentRotation();

    // Interpolate location and rotation
    FVector InterpolatedLocation = FMath::Lerp(CurrentLocation, ReplicatedLocation, Alpha);
    FRotator InterpolatedRotation = FMath::Lerp(CurrentRotation, ReplicatedRotation, Alpha);

    // Update the boat mesh
    BoatMesh->SetWorldLocation(InterpolatedLocation, true);
    BoatMesh->SetWorldRotation(InterpolatedRotation, true);

    // Reset the interpolation time when fully updated
    if (Alpha >= 1.0f)
    {
        TimeSinceLastUpdate = 0.0f;
    }
}

void UMovementBoat::Server_UpdateTransform_Implementation(float DeltaTime)
{
    if (bShouldMove)
    {
        FVector DesiredDirection = GetWorldSpaceDirection(MovementInput);
        FVector NewLocation = BoatMesh->GetComponentLocation() + (DesiredDirection * CurrentSpeed * DeltaTime);

        BoatMesh->SetWorldLocation(NewLocation, true);
        ReplicatedLocation = NewLocation;
        ReplicatedRotation = BoatMesh->GetComponentRotation();
    }
}

bool UMovementBoat::Server_UpdateTransform_Validate(float DeltaTime)
{
    return true;
}

void UMovementBoat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UMovementBoat, ReplicatedLocation);
    DOREPLIFETIME(UMovementBoat, ReplicatedRotation);
}