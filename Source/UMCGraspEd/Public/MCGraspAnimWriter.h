// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "MCAnimationDataStructure.h"
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

	static void WriteFile(const FMCAnimationData& DataStruct);
};
