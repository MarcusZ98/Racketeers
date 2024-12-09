// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Base.h"

#include "OnlineSubsystem.h"
#include "RacketeersGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APS_Base::APS_Base()
{
	// Enable replication for this actor
	bReplicates = true;
	BoatHealth = MaxBoatHealth;
	
}

void APS_Base::BeginPlay()
{
	Super::BeginPlay();
	BoatHealth = MaxBoatHealth;
}

void APS_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate PlayerInfo and each property inside if needed
	DOREPLIFETIME(APS_Base, PlayerInfo);
	DOREPLIFETIME(APS_Base, MaxBoatHealth);
	DOREPLIFETIME(APS_Base, BoatHealth);
}

void APS_Base::DamagePlayerBoat_Implementation(APlayerState* PS, int Amount)
{
	if(BoatHealth <= 0)
	{
		return;
	}
	//UE_LOG(LogTemp, Display, TEXT("Health: %f"), MaxBoatHealth);
	APS_Base* PSBase = Cast<APS_Base>(PS);
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Health:" + FString::FromInt(PSBase->MaxBoatHealth));
	if(PSBase == nullptr) return;
	PSBase->BoatHealth -= Amount;
	if(BoatHealth <= 0)
	{
		ARacketeersGameStateBase* GameState = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
		if(GameState)
		{
			GameState->AddToStats(-1, EGameStats::ALIVE, PlayerInfo.Team);
			GameState->CheckRoundEnd(PlayerInfo.Team);
		}
	}
}

bool APS_Base::DamagePlayerBoat_Validate(APlayerState* PS, int Amount)
{
	return true;
}
