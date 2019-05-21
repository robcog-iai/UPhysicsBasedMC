// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCAnimGraspReader.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

FMCAnimGraspData UMCAnimGraspReader::ReadFile(const FString& Name)
{
	FMCAnimGraspData DataStruct = FMCAnimGraspData();

	for (UMCAnimGraspDataAsset* DataAsset : LoadAllAssets())
	{
		if (DataAsset->AnimationName == Name)
		{
			DataStruct = ConvertAssetToStruct(DataAsset);
			break;
		}
	}

	return DataStruct;
}

FMCAnimGraspData UMCAnimGraspReader::ConvertAssetToStruct(const UMCAnimGraspDataAsset* DataAsset)
{
	FMCAnimGraspData DataStruct;

	DataStruct.AnimationName = DataAsset->AnimationName;
	for (FString BoneName : DataAsset->BoneNames)
	{
		DataStruct.BoneNames.Add(BoneName);
	}
	for (FMCAnimGraspFrame Frame : DataAsset->Frames)
	{
		DataStruct.AddNewPositionData(Frame);
	}

	return DataStruct;
}

TArray<UMCAnimGraspDataAsset*> UMCAnimGraspReader::LoadAllAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	while(AssetRegistry.IsLoadingAssets()) 
	{
	}

	return OnRegistryLoaded();
}

TArray<UMCAnimGraspDataAsset*> UMCAnimGraspReader::OnRegistryLoaded()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames = { TEXT("MCAnimGraspDataAsset")};
	Filter.PackagePaths.Add("/UPhysicsBasedMC/GraspingAnimations");

	TArray<FAssetData> AssetList;

	AssetRegistry.GetAssets(Filter, AssetList);

	TArray<UMCAnimGraspDataAsset*> GraspAssets;

	for (const FAssetData& DataAsset : AssetList) {
		UObject* Obj = DataAsset.GetAsset();
		if (Obj->GetClass()->IsChildOf(UMCAnimGraspDataAsset::StaticClass())) {
			GraspAssets.Add(Cast<UMCAnimGraspDataAsset>(Obj));
		}
	}

	return GraspAssets;
}