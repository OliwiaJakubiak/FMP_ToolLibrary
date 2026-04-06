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
	if (!UToolMenus::IsToolMenuUIEnabled()) return;

	TArray<FName> MenuTargets = {
		FName("LevelEditor.MainMenu"),
		FName("MainFrame.MainMenu")
	};

	for (const FName& MenuName : MenuTargets)
	{
		UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu(MenuName);
		if (!MainMenu) continue;

		if (!MainMenu->ContainsSection("OliwiasDevTools"))
		{
			// If this plugin is first alphabetically - create menu shell
			FToolMenuSection& Section = MainMenu->AddSection(
				"OliwiasDevTools",
				TAttribute<FText>(),
				FToolMenuInsert("Help", EToolMenuInsertType::After)
			);
			Section.AddSubMenu(
				"OliwiaDevToolsMenu",
				LOCTEXT("MainBtn_Label", "Oliwia's DevTools"),
				LOCTEXT("MainBtn_Tooltip", "Custom pipeline and organisation tools"),
				FNewToolMenuDelegate::CreateRaw(this, &FAssetOrganiserModule::FillMenu)
			);
			UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: Menu created by AssetOrganiser"));
		}
		else
		{
			// If another plugin already created the menu shell - wrap its delegate so both are called when submenu open
			FToolMenuSection* ExistingSection = MainMenu->FindSection("OliwiasDevTools");
			if (!ExistingSection) continue;
			FToolMenuEntry* ExistingEntry = ExistingSection->FindEntry("OliwiaDevToolsMenu");
			if (!ExistingEntry) continue;

			FNewToolMenuDelegate PreviousDelegate = ExistingEntry->SubMenuData.ConstructMenu.NewToolMenu;
			ExistingEntry->SubMenuData.ConstructMenu.NewToolMenu = FNewToolMenuDelegate::CreateLambda(
				[PreviousDelegate, this](UToolMenu* Menu)
				{
					// Call the previous plugin's delegate first (preserves existing entries)
					if (PreviousDelegate.IsBound()) PreviousDelegate.Execute(Menu);
					// Then add this plugin's entries
					FAssetOrganiserModule::FillMenu(Menu);
				}
			);
			UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: AssetOrganiser added onto existing menu"));
		}
	}
}

void FAssetOrganiserModule::FillMenu(UToolMenu* Menu)
{
	// --
	// CATEGORY: "Organisation"
	// To place this tool under a different category, change the section name and label below 
	// If anohter tool shares this category use the identical section name and it will group automatically
	// --
	if (!Menu->ContainsSection("OliwiaDevTools_Organisation"))
	{
		Menu->AddSection(
			"OliwiaDevTools_Organisation",
			LOCTEXT("OrganisationSection_Label", "Organisation")
		);
	}
		FToolMenuSection* Section = Menu->FindSection("OliwiaDevTools_Organisation");
		if (!Section) return;

		Section->AddMenuEntry(
			"SmartAssetOrganiser",
			LOCTEXT("OrganiserBtn_Label", "Smart Asset Organiser"),
			LOCTEXT("OrganiserBtn_Tooltip", "Opens the organisation utility widget"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FAssetOrganiserModule::TriggerAssetOrganiser))
		);
		UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: AssetOrganiser entry added"));
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