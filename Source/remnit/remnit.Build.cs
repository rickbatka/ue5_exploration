// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class remnit : ModuleRules
{
	public remnit(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "AITestSuite", "AITestSuite" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
