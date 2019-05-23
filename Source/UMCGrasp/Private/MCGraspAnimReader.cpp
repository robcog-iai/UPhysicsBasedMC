// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspAnimReader.h"
#include "ConfigCacheIni.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "AssetRegistry/Public/AssetRegistryModule.h"
#include "Engine/ObjectLibrary.h"

// Get data asset, returns nullptr if not found
UMCGraspAnimDataAsset* UMCGraspAnimReader::GetAnimGraspDataAsset(const FString& Name)
{
	for (UMCGraspAnimDataAsset* DataAsset : LoadAllAssets())
	{
		if (DataAsset->Name.Equals(Name))
		{
			return DataAsset;
		}
	}
	return nullptr;
}


// Read frames from the data asset file
bool UMCGraspAnimReader::ReadFramesFromName(const FString& Name, TArray<FMCGraspAnimFrameData>& OutFrames)
{
	for (UMCGraspAnimDataAsset* DataAsset : LoadAllAssets())
	{
		if (DataAsset->Name.Equals(Name))
		{
			OutFrames = DataAsset->Frames;
			return true;
		}
	}
	return false;
}


TArray<UMCGraspAnimDataAsset*> UMCGraspAnimReader::LoadAllAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	while(AssetRegistry.IsLoadingAssets()) 
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Loading.."), *FString(__func__), __LINE__);
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