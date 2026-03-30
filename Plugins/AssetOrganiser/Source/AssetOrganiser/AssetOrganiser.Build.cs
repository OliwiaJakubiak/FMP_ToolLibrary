// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AssetOrganiser : ModuleRules
{
	public AssetOrganiser(ReadOnlyTargetRules Target) : base(Target)
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
				"Engine",
				"Slate",
				"SlateCore",
				"Core",
				"InputCore",
				"AssetRegistry", // For accessing and scanning asset data
				"EditorScriptingUtilities", // For editor scripting capabilities (renaming assets, moving assets, etc.)
				"UnrealEd", // For editor functionalities (asset management, etc.)
				"Blutility", // For Blueprint utilities
				"LevelEditor", // For level editor functionalities (required to access the toolbar and menu extensions)
				"UMG", // For UI development (if you are creating custom UI for your asset organiser)
				"UMGEditor", // For UMG editor functionalities (if you are creating custom UI for your asset organiser)
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
