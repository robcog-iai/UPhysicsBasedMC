// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCWrite.h"
#include "MCRead.h"
#include "MCGraspDataAssetFactory.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

void UMCWrite::WriteFile(const FMCAnimationData& DataStruct)
{
	// If asset alread exists, edit it
	if (UMCRead::ReadNames(DataStruct.SkeletalName).Contains(DataStruct.AnimationName))
	{
		for (UMCGraspDataAsset* DataAsset : UMCRead::LoadAllAssets())
		{
			if (DataAsset->SkeletalName == DataStruct.SkeletalName && DataAsset->AnimationName == DataStruct.AnimationName)
			{
				DataAsset->BoneNames = TArray<FString>();
				for (FString BoneName : DataStruct.BoneNames)
				{
					DataAsset->BoneNames.Add(BoneName);
				}
				DataAsset->PositionEpisode = TArray<FMCEpisodeData>();
				for (FMCEpisodeData Episode : DataStruct.PositionEpisode)
				{
					DataAsset->PositionEpisode.Add(Episode);
				}
				break;
			}
		}
	}

	// Else make a new one
	auto Factory = NewObject<UMCGraspDataAssetFactory>();

	Factory->AddGraspDataAsset(DataStruct);
}