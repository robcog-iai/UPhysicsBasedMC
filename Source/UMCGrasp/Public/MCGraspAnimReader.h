// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "MCGraspAnimDataAsset.h"

/**
 * This Class is used to read animations into files
 */
class UMCGRASP_API UMCGraspAnimReader
{
public:
	// Read frames from the data asset file
	static bool ReadFramesFromName(const FString& Name, TArray<FMCGraspAnimFrameData>& OutFrames);

	// Find the grasp animation data asset, return nullptr if not found
	static UMCGraspAnimDataAsset* GetAnimGraspDataAsset(const FString& Name);

	//// Converts grasp DataAssets into grasp structs
	//static FMCGraspAnimData ConvertAssetToStruct(const UMCGraspAnimDataAsset* DataAsset);

	// Returns all GraspDataAssets that can be read
	static TArray<UMCGraspAnimDataAsset*> LoadAllAssets();

private:

	// Finds all the GraspDataAssets in a hardcoded folder
	static TArray<UMCGraspAnimDataAsset*> OnRegistryLoaded();
};
