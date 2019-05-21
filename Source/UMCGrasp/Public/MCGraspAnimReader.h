// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "MCGraspAnimStructs.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/LocalTimestampDirectoryVisitor.h"
#include "MCGraspAnimDataAsset.h"

/**
 * This Class is used to read animations into files
 */
class UMCGRASP_API UMCGraspAnimReader
{
public:
	
	// Finds grasp from animation name. 
	static FMCGraspAnimData ReadFile(const FString& Name);

	// Converts grasp DataAssets into grasp structs
	static FMCGraspAnimData ConvertAssetToStruct(const UMCGraspAnimDataAsset* DataAsset);

	// Returns all GraspDataAssets that can be read
	static TArray<UMCGraspAnimDataAsset*> LoadAllAssets();

private:

	// Finds all the GraspDataAssets in a hardcoded folder
	static TArray<UMCGraspAnimDataAsset*> OnRegistryLoaded();
};
