// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TestUE4_01 : ModuleRules
{
	public TestUE4_01(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });


		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "InputCore" });
	}
}
