// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "MCGraspAnimStructs.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/LocalTimestampDirectoryVisitor.h"
#include "CoreMinimal.h"
#include "MCGraspDataAsset.h"

/**
 * This Class is used to read animations into files
 */
class UMCGRASP_API UMCGraspAnimReader
{
public:
	
	// Finds grasp from animation name. 
	static FMCAnimationData ReadFile(const FString& Name);

	// Converts grasp DataAssets into grasp structs
	static FMCAnimationData ConvertAssetToStruct(const UMCGraspDataAsset* DataAsset);

	// Returns all GraspDataAssets that can be read
	static TArray<UMCGraspDataAsset*> LoadAllAssets();

private:

	// Finds all the GraspDataAssets in a hardcoded folder
	static TArray<UMCGraspDataAsset*> OnRegistryLoaded();
};
