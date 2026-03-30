// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "Modules/ModuleManager.h" // For module implementation
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
	// handle menu creation
	void AddMenuBarExtension(FMenuBarBuilder& Builder);
	void FillMenu(FMenuBuilder& Builder);

	// launches the editor utility widget
	void TriggerAssetOrganiser();

	TSharedPtr<FExtender> MenuExtender;
};