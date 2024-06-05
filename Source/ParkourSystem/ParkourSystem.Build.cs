// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ParkourSystem : ModuleRules
{
    public ParkourSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
            //Default
            "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "GameplayTags",
                "MotionWarping",
                "UMG",
                "ParkourSystemPlugin"
        });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                    "Projects",
                    "InputCore",
                    "EditorFramework",
                    "UnrealEd",
                    "ToolMenus",
                    "CoreUObject",
                    "Engine",
                    "Slate",
                    "SlateCore",
                // ... add private dependencies that you statically link with here ...	
            }
         );
    }
}
