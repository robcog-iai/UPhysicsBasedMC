// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "MCAnimGraspStructs.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/LocalTimestampDirectoryVisitor.h"
#include "MCAnimGraspDataAsset.h"

/**
 * This Class is used to read animations into files
 */
class UMCGRASP_API UMCAnimGraspReader
{
public:
	
	// Finds grasp from animation name. 
	static FMCAnimGraspData ReadFile(const FString& Name);

	// Converts grasp DataAssets into grasp structs
	static FMCAnimGraspData ConvertAssetToStruct(const UMCAnimGraspDataAsset* DataAsset);

	// Returns all GraspDataAssets that can be read
	static TArray<UMCAnimGraspDataAsset*> LoadAllAssets();

private:

	// Finds all the GraspDataAssets in a hardcoded folder
	static TArray<UMCAnimGraspDataAsset*> OnRegistryLoaded();
};
