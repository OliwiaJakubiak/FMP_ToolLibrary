// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuickLightingKit.h" // For the function definitions
#include "EngineUtils.h" // For iterating over actors in the world
#include "Engine/DirectionalLight.h" // For ADirectionalLight
#include "Engine/SkyLight.h" // For ASkyLight
#include "Components/LightComponent.h" // For ULightComponent
#include "Components/SkyLightComponent.h" // For USkyLightComponent
#include "Engine/ExponentialHeightFog.h" // For AExponentialHeightFog
#include "Components/ExponentialHeightFogComponent.h" // For UExponentialHeightFogComponent

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

#define LOCTEXT_NAMESPACE "FQuickLightingKitModule"

class FQuickLightingKitModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FQuickLightingKitModule, QuickLightingKit)

#undef LOCTEXT_NAMESPACE