// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
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