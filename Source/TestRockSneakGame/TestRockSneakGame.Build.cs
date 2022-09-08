// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TestRockSneakGame : ModuleRules
{
	public TestRockSneakGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"NavigationSystem", // AI
			"AIModule" // AI
		});
	}
}
