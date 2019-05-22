// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdAnimWriter.h"
#include "MCGraspAnimReader.h"
#include "MCGraspEdAnimDataAssetFactory.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

void UMCGraspEdAnimWriter::WriteToDataAsset(const FMCGraspAnimData& DataStruct)
{
	// If asset alread exists, edit it
	for (UMCGraspAnimDataAsset* DataAsset : UMCGraspAnimReader::LoadAllAssets())
	{
		if (DataAsset->AnimationName == DataStruct.Name)
		{
			DataAsset->BoneNames = TArray<FString>();
			for (FString BoneName : DataStruct.BoneNames)
			{
				DataAsset->BoneNames.Add(BoneName);
			}
			DataAsset->Frames = TArray<FMCGraspAnimFrameData>();
			for (FMCGraspAnimFrameData Frame : DataStruct.Frames)
			{
				DataAsset->Frames.Add(Frame);
			}
			return;
		}
	}

	// Else make a new one
	auto Factory = NewObject<UMCGraspEdAnimDataAssetFactory>();

	Factory->AddGraspDataAsset(DataStruct);
}