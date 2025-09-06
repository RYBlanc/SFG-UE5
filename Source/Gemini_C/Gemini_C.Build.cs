// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Gemini_C : ModuleRules
{
	public Gemini_C(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",
            "RenderCore",
            "RHI",
            "Landscape",
            "Foliage",
            "Niagara",
            "NiagaraCore",
            "Chaos",
            "PhysicsCore",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "CommonUI",
            "CommonInput"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Json",
			"HTTP"
		});

		// Editor-only dependencies
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"UnrealEd",
				"ToolMenus",
				"EditorStyle",
				"EditorWidgets"
			});
		}

        // Additional modules for advanced features
        PublicDependencyModuleNames.AddRange(new string[] {
            "HeadMountedDisplay"
        });


		// For data analysis and machine learning
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"Analytics",
				"AnalyticsET"
			});
		}
	}
}
