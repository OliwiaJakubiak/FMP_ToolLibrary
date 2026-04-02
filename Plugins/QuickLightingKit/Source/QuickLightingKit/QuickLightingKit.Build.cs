// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class QuickLightingKit : ModuleRules
{
	public QuickLightingKit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine", // Essential for lights and fog actors
				"Slate",
				"SlateCore",
				"Core",
                "InputCore", // For input handling
				"EditorScriptingUtilities", // For editor scripting capabilities (renaming assets, moving assets, etc.)
				"UnrealEd" // For editor functionalities (asset management, etc.)
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
