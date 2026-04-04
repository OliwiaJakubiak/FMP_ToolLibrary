// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "Modules/ModuleManager.h" // For module implementation
#include "ToolMenus.h" // For menu extension
#include "AssetOrganiser.generated.h"

// Passing folder and prefix rules from UI to C++ code
USTRUCT(BlueprintType)
struct FAssetOrganiserRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FolderName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Prefix;
};

UCLASS()
class ASSETORGANISER_API UAssetOrganiserFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// The core logic: takes an array of assets and an array of rules, and returns a map of assets to their new paths
	UFUNCTION(BlueprintCallable, Category = "AssetOrganiser")
	static int32 BatchOrganiseAssets_CPP(const TArray<FAssetData>& SelectedAssets, const TMap<UClass*, FAssetOrganiserRule>& OrganiserRules, bool bIsDryRun);
};

// The Module implementation is required to register the module with Unreal Engine
class FAssetOrganiserModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus(); // Registers the menu extension to add the "Smart Asset Organiser" entry to the editor's menu
	void FillMenu(UToolMenu* Menu); // Fills the submenu with entries, currently just the "Smart Asset Organiser" entry but can be expanded in the future
	void TriggerAssetOrganiser(); // Loads and launches the editor utility widget when the "Smart Asset Organiser" entry is clicked
};