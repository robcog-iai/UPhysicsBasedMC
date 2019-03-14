// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "MCAnimationDataStructure.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/LocalTimestampDirectoryVisitor.h"
#include "CoreMinimal.h"
#include "MCGraspDataAsset.h"

/**
 * This Class is used to read animations into files
 */
class UMCGRASP_API UMCRead
{
public:
	/*
	This function reads a ini file. It should be located in: GameDir/GraspAnimations/SkeletalMeshName/Name.ini
	@param SkeletalMeshName, this represents the directory
	@param Name, the file name in this directory
	@return the animation 
	*/
	static FMCAnimationData ReadFile(const FString& SkeletalMeshName, const FString& Name);

	static FMCAnimationData ConvertAssetToStruct(const UMCGraspDataAsset* DataAsset);

	/*
	This function reads all posibility animations for one skeleton 
	(e.g it reads all animations in GameDir/GraspAnimations/SkeletalMeshName/...)
	@param SkeletalMeshName, the directory that should be searched
	@return all animations
	*/
	static TArray<FString> ReadNames(const FString& SkeletalMeshName);

	static TArray<UMCGraspDataAsset*> LoadAllAssets();

private:

	static TArray<UMCGraspDataAsset*> OnRegistryLoaded();
};
