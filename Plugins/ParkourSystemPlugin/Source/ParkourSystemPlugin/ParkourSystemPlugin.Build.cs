using System.IO;
using UnrealBuildTool;

public class ParkourSystemPlugin : ModuleRules
{
    public ParkourSystemPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Public/Actors"),
                Path.Combine(ModuleDirectory, "Public/AnimationNotifies"),
                Path.Combine(ModuleDirectory, "Public/Components"),
                Path.Combine(ModuleDirectory, "Public/DataAssets"),
                Path.Combine(ModuleDirectory, "Public/FunctionLibraries"),
                Path.Combine(ModuleDirectory, "Public/Interfaces")
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private"),
                Path.Combine(ModuleDirectory, "Private/Actors"),
                Path.Combine(ModuleDirectory, "Private/AnimationNotifies"),
                Path.Combine(ModuleDirectory, "Private/Components"),
                Path.Combine(ModuleDirectory, "Private/DataAssets"),
                Path.Combine(ModuleDirectory, "Private/FunctionLibraries"),
                Path.Combine(ModuleDirectory, "Private/Interfaces")
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "GameplayTags",
                "MotionWarping",
                "UMG"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
                "InputCore",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
         );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "EditorFramework",
                    "UnrealEd",
                    "ToolMenus",
                }
            );
        }
    }
}
