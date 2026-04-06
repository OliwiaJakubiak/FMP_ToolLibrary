// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuickLightingKit.h" // For the function definitions
#include "EngineUtils.h" // For iterating over actors in the world
#include "Engine/DirectionalLight.h" // For ADirectionalLight
#include "Engine/SkyLight.h" // For ASkyLight
#include "Components/LightComponent.h" // For ULightComponent
#include "Components/SkyLightComponent.h" // For USkyLightComponent
#include "Engine/ExponentialHeightFog.h" // For AExponentialHeightFog
#include "Components/ExponentialHeightFogComponent.h" // For UExponentialHeightFogComponent
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

#define LOCTEXT_NAMESPACE "FQuickLightingKitModule"

IMPLEMENT_MODULE(FQuickLightingKitModule, QuickLightingKit);

void FQuickLightingKitModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: QuickLightingKit module started"));
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FQuickLightingKitModule::RegisterMenus)
	);
}

void FQuickLightingKitModule::RegisterMenus()
{
	if (!UToolMenus::IsToolMenuUIEnabled()) return;

	TArray<FName> MenuTargets = {
		FName("LevelEditor.MainMenu"),
		FName("MainFrame.MainMenu")
	};

	for (const FName& MenuName : MenuTargets)
	{
		UToolMenu* MainMenu = UToolMenus::Get() ->ExtendMenu(MenuName);
		if (!MainMenu) continue;

		if (!MainMenu->ContainsSection("OliwiasDevTools"))
		{
			// If plugin is first alphabetically - create menu shell
			FToolMenuSection& Section = MainMenu->AddSection(
				"OliwiasDevTools",
				TAttribute<FText>(),
				FToolMenuInsert("Help", EToolMenuInsertType::After)
			);
			Section.AddSubMenu(
				"OliwiaDevToolsMenu",
				LOCTEXT("MainBtn_Label", "Oliwia's DevTools"),
				LOCTEXT("MainBtn_Tooltip", "Custom pipeline and organisation tools"),
				FNewToolMenuDelegate::CreateRaw(this, &FQuickLightingKitModule::FillMenu)
			);
			UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: Menu created by QuickLightingKit"));
		}
		else
		{
			// If another plugin already created menu shell - wrap its delegate so both called when submenu open
			FToolMenuSection* ExistingSection = MainMenu->FindSection("OliwiasDevTools");
			if (!ExistingSection) continue;
			FToolMenuEntry* ExistingEntry = ExistingSection->FindEntry("OliwiaDevToolsMenu");
			if (!ExistingEntry) continue;

			FNewToolMenuDelegate PreviousDelegate = ExistingEntry->SubMenuData.ConstructMenu.NewToolMenu;
			ExistingEntry->SubMenuData.ConstructMenu.NewToolMenu = FNewToolMenuDelegate::CreateLambda(
				[PreviousDelegate, this](UToolMenu* Menu)
				{
					// Call previous plugin's delegate first (preserves existing entries)
					if (PreviousDelegate.IsBound()) PreviousDelegate.Execute(Menu);
					// Then add this plugins entries
					FQuickLightingKitModule::FillMenu(Menu);
				}
			);
			UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: QuickLightingKit added onto existing menu"));
		}
	}
}

void FQuickLightingKitModule::FillMenu(UToolMenu* Menu)
{
	// --
	// CATEGORY: "Lighting"
	// To place this tool under a different category, change the section name and label below 
	// If anohter tool shares this category use the identical section name and it will group automatically
	// --
	if (!Menu->ContainsSection("OliwiaDevTools_Lighting"))
	{
		Menu->AddSection(
			"OliwiaDevTools_Lighting",
			LOCTEXT("LightingSection_Label", "Lighting")
		);
	}
	FToolMenuSection* Section = Menu->FindSection("OliwiaDevTools_Lighting");
	if (!Section) return;

	Section->AddMenuEntry(
		"QuickLightingKitEntry",
		LOCTEXT("QuickLightinKitEntry_Label", "Quick Lighting Kit"),
		LOCTEXT("QuickLightingKitEntry_Tooltip", "Opens the Quick Lighting Kit widget"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FQuickLightingKitModule::TriggerQuickLightingKit))
	);
	UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: QuickLightingKit entry added"));
}

void FQuickLightingKitModule::TriggerQuickLightingKit()
{
	FString WidgetPath = TEXT("/QuickLightingKit/UI/EUW_LightingKit.EUW_LightingKit");
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
		UE_LOG(LogTemp, Warning, TEXT("Oliwia's DevTools: Could not find EUW at %s"), *WidgetPath);
	}
}

void FQuickLightingKitModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
}

// -- CORE LOGIC -- 

void UQuickLightingKit::ExecuteLightingUpdate(const UObject* WorldContextObject, float SunPitch, FLinearColor SunColor, float SunIntensity, float SkyIntensity, float FogDensity)
{
	if (!WorldContextObject) return;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	// Loop through all directional lights in the world and update their properties
	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		It->SetActorRotation(FRotator(SunPitch, 0.0f, 0.0f));
		if (ULightComponent* LightComp = It->GetLightComponent())
		{
			LightComp->SetLightColor(SunColor);
			LightComp->SetIntensity(SunIntensity);
		}
	}

	// Loop through all sky lights in the world and update their properties
	for (TActorIterator<ASkyLight> It(World); It; ++It)
	{
		if (USkyLightComponent* SkyComp = It->GetComponentByClass<USkyLightComponent>())
		{
			SkyComp->SetIntensity(SkyIntensity);
			SkyComp->RecaptureSky(); // Important to update the sky light after changing intensity
		}
	}

	// Loop through all exponential height fog actors in the world and update their properties
	for (TActorIterator<AExponentialHeightFog> It(World); It; ++It)
	{
		if (UExponentialHeightFogComponent* FogComp = It->GetComponentByClass<UExponentialHeightFogComponent>())
		{
			FogComp->SetFogDensity(FogDensity);
		}
	}
}

#undef LOCTEXT_NAMESPACE