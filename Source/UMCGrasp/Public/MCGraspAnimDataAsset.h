// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MCGraspAnimStructs.h"
#include "MCGraspAnimDataAsset.generated.h"


// Angular drive and editor rotation data for a bone in a given frame
USTRUCT()
struct FMCGraspAnimBoneData
{
	GENERATED_BODY()

	// Target value for the angular drive
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FRotator AngularOrientationTarget;

	// Array of local-space (relative to parent bone) rotation for each bone. Used for loading the animation in the editor
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
		FRotator BoneSpaceRotation;

	// Default ctor
	FMCGraspAnimBoneData() {}

	// Init Ctor with AngularOrientationTarget
	FMCGraspAnimBoneData(const FRotator& InAngularOrientationTarget) : AngularOrientationTarget(InAngularOrientationTarget) {}

	// Init Ctor
	FMCGraspAnimBoneData(const FRotator& InAngularOrientationTarget, const FRotator& InBoneSpaceRotation)
		: AngularOrientationTarget(InAngularOrientationTarget), BoneSpaceRotation(InBoneSpaceRotation)
	{}
};

// Contains all the bones data within a frame
USTRUCT()
struct FMCGraspAnimFrameData
{
	GENERATED_BODY()

	// Frame data, mapping from the bone name to their rotational data
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TMap<FString, FMCGraspAnimBoneData> Map;

	// Default ctor
	FMCGraspAnimFrameData() {}

	// Init ctor
	FMCGraspAnimFrameData(const TMap<FString, FMCGraspAnimBoneData>& InFrameData) : Map(InFrameData) {}
};

// Contains the data of a grasp animation
USTRUCT()
struct FMCGraspAnimData
{
	GENERATED_BODY()

	// Animation name
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FString Name;

	// Bone names
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
		TArray<FString> BoneNames;

	// The animation frames
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
		TArray<FMCGraspAnimFrameData> Frames;

	// Default ctor
	FMCGraspAnimData() {}
};

/**
 * 
 */
UCLASS()
class UMCGRASP_API UMCGraspAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//The name for this Animation
	UPROPERTY(EditAnywhere)
	FString AnimationName;

	//All bone names
	UPROPERTY(EditAnywhere)
	TArray<FString> BoneNames;

	//All frames
	UPROPERTY(EditAnywhere)
	TArray<FMCGraspAnimFrameData> Frames;
};
