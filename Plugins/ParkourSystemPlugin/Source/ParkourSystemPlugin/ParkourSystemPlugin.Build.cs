using UnrealBuildTool;

public class ParkourSystemPlugin : ModuleRules
{
    public ParkourSystemPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
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