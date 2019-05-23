// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdAnimWriter.h"
#include "MCGraspAnimReader.h"
#include "MCGraspEdAnimDataAssetFactory.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"


void UMCGraspEdAnimWriter::WriteToDataAsset(const FString& InAnimName, const TArray<FString>& InBoneNames, const TArray<FMCGraspAnimFrameData>& InFrames)
{
	if (UMCGraspAnimDataAsset* DataAsset = UMCGraspAnimReader::GetAnimGraspDataAsset(InAnimName))
	{
		DataAsset->Name = InAnimName;
		DataAsset->BoneNames = InBoneNames;
		DataAsset->Frames = InFrames;	
	}
	else
	{
		UMCGraspEdAnimDataAssetFactory* GraspAnimFactory = NewObject<UMCGraspEdAnimDataAssetFactory>();
		GraspAnimFactory->AddGraspDataAsset(InAnimName, InBoneNames, InFrames);
	}
}

