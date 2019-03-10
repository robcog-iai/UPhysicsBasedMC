// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MCAnimationDataStructure.h"
#include "MCGraspDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UMCGRASP_API UMCGraspDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	//The name for this Animation
	UPROPERTY(VisibleAnywhere)
	FString AnimationName;

	//The name for the used skeletal mesh 
	UPROPERTY(VisibleAnywhere)
	FString SkeletalName;

	//All Bone informations
	UPROPERTY(VisibleAnywhere)
	TArray<FString> BoneNames;

	//All episodes
	UPROPERTY(VisibleAnywhere)
	TArray<FMCEpisodeData> PositionEpisode;
};
