// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "MeshUtilityKit.generated.h"

// Enum for collision type passed from UI  to c++
UENUM(BlueprintType)
enum class EMeshCollisionType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisplayName = "Sphere"),
	Capsule UMETA(DisplayName = "Capsule"),
	SimpleAsComplex UMETA(DisplayName = "Simple As Complex"),
	ComplexAsSimple UMETA(DisplayName = "Complex As Simple")
};

UCLASS()
class MESHUTILITYKIT_API UMeshUtilityKitFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Resize selected static mesh actors in the viewport and permanently fixes source asset
	UFUNCTION(BlueprintCallable, Category = "MeshUtilityKit")
	static int32 ResizeSelectedMeshes_CPP(const TArray<AActor*>& SelectedActors, float TargetSize);

	// Sets pivot to Base or Centre on selected static mesh actors and source asset
	UFUNCTION(BlueprintCallable, Category = "MeshUtilityKit")
	static int32 ResetAssetToOrigin_CPP(const TArray<AActor*>& SelectedActors, bool bResetRotation);

	// Genertae collision on selected static mesh actors and source asset
	UFUNCTION(BlueprintCallable, Category = "MeshUtilityaKit")
	static int32 GenerateCollisionOnSelectedMeshes_CPP(const TArray<AActor*>& SelectedActors, EMeshCollisionType CollisionType);
};

// Module implementation required to register with UE
class FMeshUtilityKitModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private: 
	void RegisterMenus();
	void FillMenu(UToolMenu* Menu);
	void TriggerMeshUtilityKit();
};
