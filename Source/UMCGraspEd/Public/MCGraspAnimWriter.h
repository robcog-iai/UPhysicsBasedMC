// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "MCGraspAnimStructs.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/LocalTimestampDirectoryVisitor.h"
#include "CoreMinimal.h"
#include "MCGraspDataAsset.h"

/**
 * This Class is used to write animations into files
 */
class UMCGraspAnimWriter
{
public:

	// Overwrites or creates DataAsset for a specific grasp
	static void WriteFile(const FMCAnimationData& DataStruct);
};
