// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MCAnimGraspStructs.h"
#include "MCAnimGraspDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UMCGRASP_API UMCAnimGraspDataAsset : public UDataAsset
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
	TArray<FMCAnimGraspFrame> Frames;
};
