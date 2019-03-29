// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspAnimWriter.h"
#include "MCGraspAnimReader.h"
#include "MCGraspDataAssetFactory.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

void UMCGraspAnimWriter::WriteFile(const FMCAnimationData& DataStruct)
{
	// If asset alread exists, edit it
	for (UMCGraspDataAsset* DataAsset : UMCGraspAnimReader::LoadAllAssets())
	{
		if (DataAsset->AnimationName == DataStruct.AnimationName)
		{
			DataAsset->BoneNames = TArray<FString>();
			for (FString BoneName : DataStruct.BoneNames)
			{
				DataAsset->BoneNames.Add(BoneName);
			}
			DataAsset->Frames = TArray<FMCFrame>();
			for (FMCFrame Episode : DataStruct.Frames)
			{
				DataAsset->Frames.Add(Episode);
			}
			return;
		}
	}

	// Else make a new one
	auto Factory = NewObject<UMCGraspDataAssetFactory>();

	Factory->AddGraspDataAsset(DataStruct);
}