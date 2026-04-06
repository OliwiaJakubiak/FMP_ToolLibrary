// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h" // For basic types and macros
#include "Kismet/BlueprintFunctionLibrary.h" // For UFUNCTION(BlueprintCallable)
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "QuickLightingKit.generated.h" // Must be the last include

UCLASS()
class QUICKLIGHTINGKIT_API UQuickLightingKit : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// This node handles the heavy lifting of finding and updating world actors 
	// Everything is passed in from blueprint data table 
	UFUNCTION(BlueprintCallable, Category = "ToolLibrary|QuickLightingKit", meta = (WorldContext = "WorldContextObject"))
	static void ExecuteLightingUpdate(
		const UObject* WorldContextObject,
		float SunPitch,
		FLinearColor SunColor,
		float SunIntensity,
		float SkyIntensity,
		float FogDensity);
};

class FQuickLightingKitModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private: 
	void RegisterMenus();
	void FillMenu(UToolMenu* Menu);
	void TriggerQuickLightingKit();
};