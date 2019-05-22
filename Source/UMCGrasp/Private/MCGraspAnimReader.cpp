// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspAnimReader.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

FMCGraspAnimData UMCGraspAnimReader::ReadFile(const FString& Name)
{
	FMCGraspAnimData DataStruct = FMCGraspAnimData();

	for (UMCGraspAnimDataAsset* DataAsset : LoadAllAssets())
	{
		if (DataAsset->AnimationName == Name)
		{
			DataStruct = ConvertAssetToStruct(DataAsset);
			break;
		}
	}

	return DataStruct;
}

FMCGraspAnimData UMCGraspAnimReader::ConvertAssetToStruct(const UMCGraspAnimDataAsset* DataAsset)
{
	FMCGraspAnimData DataStruct;

	DataStruct.Name = DataAsset->AnimationName;
	for (FString BoneName : DataAsset->BoneNames)
	{
		DataStruct.BoneNames.Add(BoneName);
	}
	for (FMCGraspAnimFrameData Frame : DataAsset->Frames)
	{
		DataStruct.Frames.Add(Frame);
	}

	return DataStruct;
}

TArray<UMCGraspAnimDataAsset*> UMCGraspAnimReader::LoadAllAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	while(AssetRegistry.IsLoadingAssets()) 
	{
	}

	return OnRegistryLoaded();
}

TArray<UMCGraspAnimDataAsset*> UMCGraspAnimReader::OnRegistryLoaded()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames = { TEXT("MCGraspAnimDataAsset")};
	Filter.PackagePaths.Add("/UPhysicsBasedMC/GraspingAnimations");

	TArray<FAssetData> AssetList;

	AssetRegistry.GetAssets(Filter, AssetList);

	TArray<UMCGraspAnimDataAsset*> GraspAssets;

	for (const FAssetData& DataAsset : AssetList) {
		UObject* Obj = DataAsset.GetAsset();
		if (Obj->GetClass()->IsChildOf(UMCGraspAnimDataAsset::StaticClass())) {
			GraspAssets.Add(Cast<UMCGraspAnimDataAsset>(Obj));
		}
	}

	return GraspAssets;
}