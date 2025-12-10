// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CH4TopDownProject : ModuleRules
{
	public CH4TopDownProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"NavigationSystem", 
			"AIModule", 
			"Niagara", 
			"EnhancedInput",
            "UMG",
        });

        PublicIncludePaths.AddRange(new string[] 
		{ 
			"CH4TopDownProject", 
		});
    }
}
