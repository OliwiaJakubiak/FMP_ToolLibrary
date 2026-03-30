// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetOrganiser.h"
#include "LevelEditor.h" // For menu extension
#include "EditorUtilityWidgetBlueprint.h" // For launching the editor utility widget
#include "EditorUtilitySubsystem.h" // For launching the editor utility widget
#include "EditorAssetLibrary.h" // Requires EditorScriptingUtilities module
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FAssetOrganiserModule" // For logging purposes

IMPLEMENT_MODULE(FAssetOrganiserModule, AssetOrganiser);

void FAssetOrganiserModule::StartupModule()
{
	// Register menu extension
	// Get Level Editor module to extend the menu
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	// Create a menu extender
	MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuBarExtension(
		"Help", // Right after menu help menu, change as needed
		EExtensionHook::After,
		nullptr,
		FMenuBarExtensionDelegate::CreateRaw(this, &FAssetOrganiserModule::AddMenuBarExtension));
	// Add the extender to the level editor
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FAssetOrganiserModule::AddMenuBarExtension(FMenuBarBuilder& Builder)
{
	// Creates a new menu entry 
	Builder.AddPullDownMenu(
		LOCTEXT("MainBtn_Label", "Oliwia's DevTools"), // Menu label
		LOCTEXT("MainBtn_Tooltip", "Custom pipeline and organisation tools"), // Menu tooltip
		FNewMenuDelegate::CreateRaw(this, &FAssetOrganiserModule::FillMenu), // Delegate to fill the menu
		"OliwiaDevTools" // ID, shared across all plugins in the same menu, change as needed
	);
}

void FAssetOrganiserModule::FillMenu(FMenuBuilder& Builder)
{
	// Adds an entry to the menu that launches the editor utility widget
	Builder.AddMenuEntry(
		LOCTEXT("OrganiserBtn_Label", "Smart Asset Organiser"), // Button label)
		LOCTEXT("OrganiserBtn_Tooltip", "Opens the organisation utility widget"), // Button tooltip)
		FSlateIcon(), // Icon, can be set to a custom one if desired
		FUIAction(FExecuteAction::CreateRaw(this, &FAssetOrganiserModule::TriggerAssetOrganiser)) // Delegate to trigger the widget)
	);
}

void FAssetOrganiserModule::TriggerAssetOrganiser()
{
	// Load and launch the editor utility widget
	FString WidgetPath = TEXT("/AssetOrganiser/UI/EUW_AssetOrganiser.EUW_AssetOrganiser"); // Path to the widget blueprint, change as needed)

	UObject* WidgetObj = StaticLoadObject(UEditorUtilityWidgetBlueprint::StaticClass(), nullptr, *WidgetPath);

	if (WidgetObj)
	{
		UEditorUtilityWidgetBlueprint* WidgetBP = Cast<UEditorUtilityWidgetBlueprint>(WidgetObj);
		if (UEditorUtilitySubsystem* Subsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
		{
			Subsystem->SpawnAndRegisterTab(WidgetBP);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Oliwia's DevTools: Could not find EUW at %s"), *WidgetPath);
	}
}

void FAssetOrganiserModule::ShutdownModule()
{
	// Cleanup if necessary
}

// -- CORE LOGIC: takes an array of assets and an array of rules, and returns a map of assets to their new paths --

int32 UAssetOrganiserFunctionLibrary::BatchOrganiseAssets_CPP(const TArray<FAssetData>& SelectedAssets, const TMap<UClass*, FAssetOrganiserRule>& OrganiserRules, bool bIsDryRun)
{
	int32 SuccessCount = 0;

	for (const FAssetData& Asset : SelectedAssets)
	{
		UClass* AssetClass = Asset.GetClass();

		// Check if there is a rule for this asset's class
		if (OrganiserRules.Contains(AssetClass))
		{
			const FAssetOrganiserRule& Rule = OrganiserRules[AssetClass];

			// Build the new path for the asset: {Folder}/{Sub}/{Prefix}_{Name}
			FString OldPath = Asset.PackagePath.ToString();
			FString NewPath = FString::Printf(TEXT("%s/%s/%s%s"),
				*OldPath,
				*Rule.FolderName,
				*Rule.Prefix,
				*Asset.AssetName.ToString());

			if (bIsDryRun)
			{
				UE_LOG(LogTemp, Warning, TEXT("DRY RUN: Moving asset '%s' to '%s'"), *Asset.GetFullName(), *NewPath);
				continue;
			}

			// Perform actual move and rename
			if (UEditorAssetLibrary::RenameAsset(Asset.PackageName.ToString(), NewPath))
			{
				SuccessCount++;
			}
		}
	}

	return SuccessCount;
}

#undef LOCTEXT_NAMESPACE