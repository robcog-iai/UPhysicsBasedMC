// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCAnimGraspDataAssetFactory.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

UMCGraspDataAssetFactory::UMCGraspDataAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMCAnimGraspDataAsset::StaticClass();

	bEditorImport = true;

	ImportPriority = DefaultImportPriority;
}

UObject* UMCGraspDataAssetFactory::CreateGraspDataAsset(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, FMCAnimGraspData DataStruct)
{
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);

	UMCAnimGraspDataAsset* DataAsset = NewObject<UMCAnimGraspDataAsset>(InParent, InName, Flags);

	DataAsset->AnimationName = DataStruct.AnimationName;
	for (FString BoneName : DataStruct.BoneNames)
	{
		DataAsset->BoneNames.Add(BoneName);
	}
	for(FMCAnimGraspFrame Frame : DataStruct.Frames)
	{
		DataAsset->Frames.Add(Frame);
	}

	FEditorDelegates::OnAssetPostImport.Broadcast(this, DataAsset);

	return DataAsset;
}

void UMCGraspDataAssetFactory::AddGraspDataAsset(const FMCAnimGraspData& DataStruct)
{
	FString FinalPackageName = "/UPhysicsBasedMC/GraspingAnimations/" + DataStruct.AnimationName;

	UPackage* Package = CreatePackage(NULL, *FinalPackageName);

	UPackage* OutermostPkg = Package->GetOutermost();

	UObject* DataAsset = CreateGraspDataAsset(UMCAnimGraspDataAsset::StaticClass(), OutermostPkg, FName(*DataStruct.AnimationName), RF_Standalone | RF_Public, NULL, DataStruct);

	FAssetRegistryModule::AssetCreated(DataAsset);
	DataAsset->MarkPackageDirty();
	Package->SetDirtyFlag(true);
	DataAsset->PostEditChange();
	DataAsset->AddToRoot();
}