// Fill out your copyright notice in the Description page of Project Settings.


#include "ULevelSubSystem.h"


//Load a level to the server and all the clients, but runs after ServerUnloadLevel is finished
void UULevelSubSystem::ServerLoadLevel()
{
	//Load Level on server first
	// when Loading finished call LoadOnAllClients
	
}

//Unload the level first on the server, and when it is finished call it on the clients
void UULevelSubSystem::ServerUnloadLevel()
{

	
}

void UULevelSubSystem::LoadOnAllClients()
{

	//Call on player controller to load a leve
	//
	
}

void UULevelSubSystem::UnloadOnAllClients()
{

	
}
