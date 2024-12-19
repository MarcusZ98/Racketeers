// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Racketeers : ModuleRules
{
	public Racketeers(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara", "AdvancedSteamSessions", "AdvancedSteamSessions", "PCG", "PCG" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "EnhancedInput" , "UMG", "Slate", "SlateCore", "HeadMountedDisplay", "AdvancedSteamSessions", "AdvancedSessions"});
	}
}
