// Fill out your copyright notice in the Description page of Project Settings.


#include "RacketeersGameStateBase.h"

void ARacketeersGameStateBase::SetMaterial(const FResources& Materials, Teams Team)
{
	if(Team == Teams::TEAM_A)
	{
		Team_A_Materials = Materials;
	}
	Team_B_Materials = Materials;
	
}

FResources ARacketeersGameStateBase::GetResources(Teams Team)
{
	if(Team == Teams::TEAM_A)
	{
		return  Team_A_Materials;
	}
	return  Team_B_Materials;
	
}