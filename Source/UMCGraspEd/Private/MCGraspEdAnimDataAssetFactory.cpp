// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdAnimDataAssetFactory.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

UMCGraspEdAnimDataAssetFactory::UMCGraspEdAnimDataAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMCGraspAnimDataAsset::StaticClass();

	bEditorImport = true;

	ImportPriority = DefaultImportPriority;
}

UObject* UMCGraspEdAnimDataAssetFactory::CreateGraspDataAsset(UClass* InClass, 
	UObject* InParent, 
	FName InName, 
	EObjectFlags Flags, 
	const TCHAR* Parms, 
	const TArray<FMCGraspAnimFrameData>& InFrames,
	const TArray<FString>& InBoneNames)
{
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);

	UMCGraspAnimDataAsset* DataAsset = NewObject<UMCGraspAnimDataAsset>(InParent, InName, Flags);

	DataAsset->Name = InName.ToString();
	//DataAsset->BoneNames = InBoneNames;
	DataAsset->Frames = InFrames;

	FEditorDelegates::OnAssetPostImport.Broadcast(this, DataAsset);

	return DataAsset;
}

void UMCGraspEdAnimDataAssetFactory::AddGraspDataAsset(const FString& InAnimName,
	const TArray<FString>& InBoneNames,
	const TArray<FMCGraspAnimFrameData>& InFrames)
{
	FString FinalPackageName = "/UPhysicsBasedMC/GraspingAnimations/" + InAnimName;

	UPackage* Package = CreatePackage(NULL, *FinalPackageName);

	UPackage* OutermostPkg = Package->GetOutermost();

	UObject* DataAsset = CreateGraspDataAsset(UMCGraspAnimDataAsset::StaticClass(),
		OutermostPkg, FName(*InAnimName), RF_Standalone | RF_Public, NULL, InFrames, InBoneNames);

	FAssetRegistryModule::AssetCreated(DataAsset);
	DataAsset->MarkPackageDirty();
	Package->SetDirtyFlag(true);
	DataAsset->PostEditChange();
	DataAsset->AddToRoot();
}