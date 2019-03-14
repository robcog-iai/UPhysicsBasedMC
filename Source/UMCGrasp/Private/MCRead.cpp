// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCRead.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

FMCAnimationData UMCRead::ReadFile(const FString& SkeletalMeshName, const FString& Name)
{
	FMCAnimationData DataStruct = FMCAnimationData();

	for (UMCGraspDataAsset* DataAsset : LoadAllAssets())
	{
		if (DataAsset->SkeletalName == SkeletalMeshName && DataAsset->AnimationName == Name)
		{
			DataStruct = ConvertAssetToStruct(DataAsset);
			break;
		}
	}

	return DataStruct;
}

FMCAnimationData UMCRead::ConvertAssetToStruct(const UMCGraspDataAsset* DataAsset)
{
	FMCAnimationData DataStruct = FMCAnimationData();

	DataStruct.AnimationName = DataAsset->AnimationName;
	DataStruct.SkeletalName = DataAsset->SkeletalName;
	for (FString BoneName : DataAsset->BoneNames)
	{
		DataStruct.BoneNames.Add(BoneName);
	}
	for (FMCEpisodeData Episode : DataAsset->PositionEpisode)
	{
		DataStruct.AddNewPositionData(Episode);
	}

	return DataStruct;
}


TArray<FString> UMCRead::ReadNames(const FString& SkeletalMeshName)
{
	TArray<FString> Files;

	TArray<UMCGraspDataAsset*> DataAssets = LoadAllAssets();

	for(UMCGraspDataAsset* DataAsset : DataAssets)
	{
		if (DataAsset->SkeletalName == SkeletalMeshName)
		{
			Files.Add(DataAsset->AnimationName);
		}
	}

	return Files;
}

TArray<UMCGraspDataAsset*> UMCRead::LoadAllAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	while(AssetRegistry.IsLoadingAssets()) 
	{
	}

	return OnRegistryLoaded();
}

TArray<UMCGraspDataAsset*> UMCRead::OnRegistryLoaded()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames = { TEXT("MCGraspDataAsset")};
	Filter.PackagePaths.Add("/UPhysicsBasedMC/Grasps");

	TArray<FAssetData> AssetList;

	AssetRegistry.GetAssets(Filter, AssetList);

	TArray<UMCGraspDataAsset*> GraspAssets;

	for (const FAssetData& DataAsset : AssetList) {
		UObject* Obj = DataAsset.GetAsset();
		if (Obj->GetClass()->IsChildOf(UMCGraspDataAsset::StaticClass())) {
			GraspAssets.Add(Cast<UMCGraspDataAsset>(Obj));
		}
	}

	return GraspAssets;
}