// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MCGraspAnimStructs.h"
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

	//All Bone informations
	UPROPERTY(VisibleAnywhere)
	TArray<FString> BoneNames;

	//All episodes
	UPROPERTY(VisibleAnywhere)
	TArray<FMCFrame> Frames;
};
