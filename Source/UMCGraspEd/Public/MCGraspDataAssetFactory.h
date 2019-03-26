// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MCGRaspDataAsset.h"
#include "MCGraspDataAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class UMCGraspDataAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
public:

	// Create DataAsset
	UObject* CreateGraspDataAsset(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, FMCAnimationData DataStruct);

	// Listens to MCIO delegator to see when a new GraspDataAsset should be created in the editor
	void AddGraspDataAsset(const FMCAnimationData& DataStruct);
};
