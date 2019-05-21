// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MCGraspAnimDataAsset.h"
#include "MCGraspEdAnimDataAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class UMCGraspEdAnimDataAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
public:

	// Create DataAsset
	UObject* CreateGraspDataAsset(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, FMCGraspAnimData DataStruct);

	// Called by GraspAnimWriter to tell factory to create new DataAsset
	void AddGraspDataAsset(const FMCGraspAnimData& DataStruct);
};