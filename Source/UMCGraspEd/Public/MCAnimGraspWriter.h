// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "MCAnimGraspStructs.h"
#include "MCAnimGraspDataAsset.h"

/**
 * This Class is used to write animations to data assets
 */
class UMCAnimGraspWriter
{
public:
	// Overwrites or creates DataAsset for a specific grasp
	static void WriteToDataAsset(const FMCAnimGraspData& DataStruct);
};
