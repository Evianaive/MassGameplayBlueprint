using UnrealBuildTool;

public class MassGameplayScriptEditor : ModuleRules
{
    public MassGameplayScriptEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "MassGameplayScript",
                "UnrealEd"
            }
        );
    }
}