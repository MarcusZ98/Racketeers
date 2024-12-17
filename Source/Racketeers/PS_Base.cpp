// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Base.h"

#include "RacketeersController.h"
#include "RacketeersGameStateBase.h"
#include "RacketeersGMBase.h"
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

void APS_Base::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);
	PlayerState->SetIsOnlyASpectator(PlayerState->IsSpectator());
}

void APS_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate PlayerInfo and each property inside if needed
	DOREPLIFETIME(APS_Base, PlayerInfo);
	DOREPLIFETIME(APS_Base, MaxBoatHealth);
	DOREPLIFETIME(APS_Base, BoatHealth);
}

void APS_Base::DamagePlayerBoat_Implementation(APlayerState* PS, int Amount, APlayerState* Other)
{
	APS_Base* PSBase = Cast<APS_Base>(PS);
	if(PSBase == nullptr  || PSBase->BoatHealth <= 0) return;
	//UE_LOG(LogTemp, Display, TEXT("Health: %f"), MaxBoatHealth);
	GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Red, "Health:" + FString::FromInt(PSBase->BoatHealth));
	
	if(Other)
	{
		APS_Base* InstigatorBase = Cast<APS_Base>(Other);
		if(InstigatorBase)
		{
			InstigatorBase->PlayerStats.TotalDamage += Amount;
		}
	}
	
	PSBase->BoatHealth -= Amount;
	if(PSBase->BoatHealth <= 0)
	{
		ARacketeersGameStateBase* GameState = Cast<ARacketeersGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
		ARacketeersGMBase* GmBase = Cast<ARacketeersGMBase>(UGameplayStatics::GetGameMode(GetWorld()));
		if(GameState)
		{
			ARacketeersController* PlayerC = Cast<ARacketeersController>(PS->GetPlayerController());
			
			GameState->AddToStats(-1, EGameStats::ALIVE, PSBase->PlayerInfo.Team);
			GameState->CheckRoundEnd(PSBase->PlayerInfo.Team);
			GameState->RemovePlayerAlive(PSBase);
			if(PlayerC)
			{
				PlayerC->SetPlayerSpectator();
			}
		
		}
	}
}

bool APS_Base::DamagePlayerBoat_Validate(APlayerState* PS, int Amount,  APlayerState* Other)
{
	return true;
}
