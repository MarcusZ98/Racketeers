#include "DriverSteerProxy.h"
#include "Net/UnrealNetwork.h"

UDriverSteerProxy::UDriverSteerProxy()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true); // Ensure the component is replicated by default
}

void UDriverSteerProxy::BeginPlay()
{
    Super::BeginPlay();
    Boat = GetOwner();

    // Get the BoatMovement component from the boat actor
    if (Boat)
    {
        BoatMovementComponent = Boat->FindComponentByClass<UBoatMovement>();
        if (!BoatMovementComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("BoatMovementComponent not found on %s."), *Boat->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Boat not found for DriverSteerProxy."));
    }
}

void UDriverSteerProxy::HandleSteerInput(float Value)
{
    if (BoatMovementComponent)
    {
        if (!GetOwner()->HasAuthority())
        {
            if (Value >= -1.0f && Value <= 1.0f) // Validate the value on the client side before sending to the server
            {
                ServerHandleSteerInput(Value); // Request the server to handle steering
            }
        }
        else
        {
            BoatMovementComponent->Steer(Value); // Apply steering directly on the server
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BoatMovementComponent is null. Cannot handle steer input."));
    }
}

void UDriverSteerProxy::ServerHandleSteerInput_Implementation(float Value)
{
    if (BoatMovementComponent && (Value >= -1.0f && Value <= 1.0f)) // Validate the value
    {
        BoatMovementComponent->Steer(Value); // Apply steering on the server
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid steer value or BoatMovementComponent is null on server."));
    }
}

bool UDriverSteerProxy::ServerHandleSteerInput_Validate(float Value)
{
    return (Value >= -1.0f && Value <= 1.0f); // Ensure input is within the valid range
}

void UDriverSteerProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDriverSteerProxy, Boat);
    DOREPLIFETIME(UDriverSteerProxy, BoatMovementComponent);
}