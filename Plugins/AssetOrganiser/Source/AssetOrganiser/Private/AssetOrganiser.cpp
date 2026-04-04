// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetOrganiser.h"
#include "LevelEditor.h" // For menu extension
#include "EditorUtilityWidgetBlueprint.h" // For launching the editor utility widget
#include "EditorUtilitySubsystem.h" // For launching the editor utility widget
#include "EditorAssetLibrary.h" // Requires EditorScriptingUtilities module
#include "AssetRegistry/AssetRegistryModule.h"
#include "ToolMenus.h" // For menu extension

#define LOCTEXT_NAMESPACE "FAssetOrganiserModule" // For logging purposes

IMPLEMENT_MODULE(FAssetOrganiserModule, AssetOrganiser);

void FAssetOrganiserModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: Module started!"));
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetOrganiserModule::RegisterMenus));
}

void FAssetOrganiserModule::RegisterMenus()
{
	// Ensure UToolMenus is available
	if (!UToolMenus::IsToolMenuUIEnabled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: UToolMenus is not enabled. Menu extension will not work."));
		return;
	}
	// register against multiple menus 
	// level editor - main level editor menu
	// main frame - the main editor menu, appears in all contexts (including when no level is open)
	TArray<FName> MenuTargets = {
		FName("LevelEditor.MainMenu"),
		FName("MainFrame.MainMenu")
	};

	for (const FName& MenuName : MenuTargets)
	{
		UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu(MenuName);
		if (!MainMenu)
		{
			UE_LOG(LogTemp, Error, TEXT("Oliwia's DevTools: Failed to extend menu %s"), *MenuName.ToString());
			continue;
		}

		// Check if custom section exists, if not create it (avoids duplicates if multiple plugins try to add to the same section)
		if (MainMenu->ContainsSection("OliwiasDevTools"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: Menu section already exists, skipping creation"));
			continue;
		}
		// Add custom top level section for our plugin, explicitly after help section
		FToolMenuSection& Section = MainMenu->AddSection(
			"OliwiasDevTools",
			TAttribute<FText>(),
			FToolMenuInsert("Help", EToolMenuInsertType::After)
		);
		Section.AddSubMenu(
			"OliwiaDevToolsMenu",
			LOCTEXT("MainBtn_Label", "Oliwia's DevTools"), // Submenu label
			LOCTEXT("MainBtn_Tooltip", "Custom pipeline and organisation tools"), // Submenu tooltip
			FNewToolMenuDelegate::CreateRaw(this, &FAssetOrganiserModule::FillMenu) // Delegate to fill the submenu with entries
		);

		UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: Menu registered successfully"));
	}
}

void FAssetOrganiserModule::FillMenu(UToolMenu* Menu)
{
	// Each tool in the submenu gets its own section, this is mostly for visual clarity but also allows users to easily find the entry for a specific tool if we add more in the future
	FToolMenuSection& Section = Menu->AddSection(
		"OliwiaDevToolsSection",
		LOCTEXT("OliwiaDevToolsSection_Label", "Tools")
	);
	// Add Smart Asset Organiser entry to the submenu, this will launch the editor utility widget when clicked
	Section.AddMenuEntry(
		"SmartAssetOrganiser",
		LOCTEXT("OrganiserBtn_Label", "Smart Asset Organiser"),
		LOCTEXT("OrganiserBtn_Tooltip", "Opens the organisation utility widget"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FAssetOrganiserModule::TriggerAssetOrganiser))
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
	// Properly unregister all UToolMenus delegates to avoid issues with dangling pointers and ensure clean shutdown
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
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