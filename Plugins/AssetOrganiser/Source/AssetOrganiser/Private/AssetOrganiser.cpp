// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetOrganiser.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetData.h"
#include "EditorAssetLibrary.h" // Requires EditorScriptingUtilities module
#include "AssetRegistry/AssetRegistryModule.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, AssetOrganiser);

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