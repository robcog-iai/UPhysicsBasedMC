// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MCGraspAnimDataAsset.generated.h"

// Angular drive and editor rotation data for a bone in a given frame
USTRUCT()
struct FMCGraspAnimBoneOrientation
{
	GENERATED_BODY()

	// Target value for the angular drive
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FRotator AngularOrientationTarget;

	// Array of local-space (relative to parent bone) rotation for each bone. Used for loading the animation in the editor
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FRotator BoneSpaceRotation;

	//// Default ctor
	//FMCGraspAnimBoneOrientation() {}
};

// Contains all the bones data within a frame
USTRUCT()
struct FMCGraspAnimFrameData
{
	GENERATED_BODY()

	// Frame data, mapping from the bone name to their rotational data
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TMap<FString, FMCGraspAnimBoneOrientation> BonesData;

	//// Default ctor
	//FMCGraspAnimFrameData() {}
};

/**
 * Contains the data of a grasp animation
 */
UCLASS()
class UMCGRASP_API UMCGraspAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//The name for this Animation
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FString Name;

	//All frames
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TArray<FMCGraspAnimFrameData> Frames;
};
