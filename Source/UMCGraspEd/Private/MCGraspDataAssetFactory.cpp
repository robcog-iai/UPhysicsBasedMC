// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspDataAssetFactory.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

UMCGraspDataAssetFactory::UMCGraspDataAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMCGraspDataAsset::StaticClass();

	bEditorImport = true;

	ImportPriority = DefaultImportPriority;
}

UObject* UMCGraspDataAssetFactory::CreateGraspDataAsset(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, FMCAnimationData DataStruct)
{
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);

	UMCGraspDataAsset* DataAsset = NewObject<UMCGraspDataAsset>(InParent, InName, Flags);

	DataAsset->AnimationName = DataStruct.AnimationName;
	for (FString BoneName : DataStruct.BoneNames)
	{
		DataAsset->BoneNames.Add(BoneName);
	}
	for(FMCEpisodeData Episode : DataStruct.PositionEpisode)
	{
		DataAsset->PositionEpisode.Add(Episode);
	}

	FEditorDelegates::OnAssetPostImport.Broadcast(this, DataAsset);

	return DataAsset;
}

void UMCGraspDataAssetFactory::AddGraspDataAsset(const FMCAnimationData& DataStruct)
{
	FString FinalPackageName = "/UPhysicsBasedMC/GraspingAnimations/" + DataStruct.AnimationName;

	UPackage* Package = CreatePackage(NULL, *FinalPackageName);

	UPackage* OutermostPkg = Package->GetOutermost();

	UObject* DataAsset = CreateGraspDataAsset(UMCGraspDataAsset::StaticClass(), OutermostPkg, FName(*DataStruct.AnimationName), RF_Standalone | RF_Public, NULL, DataStruct);

	FAssetRegistryModule::AssetCreated(DataAsset);
	DataAsset->MarkPackageDirty();
	Package->SetDirtyFlag(true);
	DataAsset->PostEditChange();
	DataAsset->AddToRoot();
}