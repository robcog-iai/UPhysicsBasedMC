// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCAnimGraspWriter.h"
#include "MCAnimGraspReader.h"
#include "MCAnimGraspDataAssetFactory.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

void UMCAnimGraspWriter::WriteToDataAsset(const FMCAnimGraspData& DataStruct)
{
	// If asset alread exists, edit it
	for (UMCAnimGraspDataAsset* DataAsset : UMCAnimGraspReader::LoadAllAssets())
	{
		if (DataAsset->AnimationName == DataStruct.AnimationName)
		{
			DataAsset->BoneNames = TArray<FString>();
			for (FString BoneName : DataStruct.BoneNames)
			{
				DataAsset->BoneNames.Add(BoneName);
			}
			DataAsset->Frames = TArray<FMCAnimGraspFrame>();
			for (FMCAnimGraspFrame Frame : DataStruct.Frames)
			{
				DataAsset->Frames.Add(Frame);
			}
			return;
		}
	}

	// Else make a new one
	auto Factory = NewObject<UMCGraspDataAssetFactory>();

	Factory->AddGraspDataAsset(DataStruct);
}