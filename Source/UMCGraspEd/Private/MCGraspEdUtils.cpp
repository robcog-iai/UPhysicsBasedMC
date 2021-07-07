// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdUtils.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "AnimPreviewInstance.h"
#include "Interfaces/IMainFrameModule.h"
#include "IAssetRegistry.h"
#include "AssetRegistryModule.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "FMCGraspEdModule"

// Default ctor
FMCGraspEdUtils::FMCGraspEdUtils()
{
	bInitialStateCached = false;
	bIsInit = false;

	CurrEditFrameIndex = 0;
	CurrEditGraspName = "";
}

// Init component
void FMCGraspEdUtils::Init(UDebugSkelMeshComponent* InDebugMeshComponent)
{
	if (!bIsInit)
	{
		DebugMeshComponent = InDebugMeshComponent;

		if (DebugMeshComponent)
		{
			bIsInit = true;
		}
	}
}


/* Action mappings 'New Grasp' */
// Saves all the transforms we need later on, before bones are moved
void FMCGraspEdUtils::SaveMeshInitialState()
{
	if (!bIsInit)
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Helper class is not initialized.."), *FString(__func__), __LINE__);
		return;
	}

	if (!bInitialStateCached)
	{
		TArray<FName> BoneNames;
		DebugMeshComponent->GetBoneNames(BoneNames);
		int32 BoneIndex = 0;
		for (const auto& Name : BoneNames)
		{
			InitialBoneSpaceLocations.Add(Name.ToString(), DebugMeshComponent->BoneSpaceTransforms[BoneIndex].GetTranslation());
			InitialBoneCompSpaceTransf.Add(Name.ToString(), DebugMeshComponent->GetBoneTransform(BoneIndex));
			BoneIndex++;
		}

		bInitialStateCached = true; 
		ShowMessageBox(FText::FromString("Save Initial State"), FText::FromString("The initial state of the skeletal mesh has been successfully cached, you can begin adding frames.."));
	}
	else
	{
		ShowMessageBox(FText::FromString("Save Initial State"), FText::FromString("The initial state of the skeletal mesh is already cached, you can add frames.."));
	}
}

// Saves the current position of the displayed mesh to a new frame
void FMCGraspEdUtils::AddFrame()
{
	if (!bInitialStateCached) 
	{
		ShowMessageBox(FText::FromString("Add Frame"), FText::FromString("The initial state of the skeletal mesh is NOT cached"));
		return;
	}

	FMCGraspAnimFrameData NewFrameData;

	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceRotations;
	int32 BoneIndex = 0;

	// Saves the current bone rotations as bone space. Its needed to load in a frame since only bone space is displayed in a preview.
	for (const auto& BoneTransform : DebugMeshComponent->BoneSpaceTransforms)
	{
		CurrentBoneSpaceRotations.Add(BoneNames[BoneIndex].ToString(), BoneTransform.GetRotation().Rotator());
		BoneIndex++;
	}

	// go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* CI : DebugMeshComponent->Constraints)
	{
		FMCGraspAnimBoneOrientation NewBoneData;

		// Gets BoneSpaceRotation from CurrentBoneSpaceRotations
		NewBoneData.BoneSpaceRotation = *CurrentBoneSpaceRotations.Find(CI->ConstraintBone1.ToString());

		/**
		*Name of first bone(body) that this constraint is connecting.
		*This will be the 'child' bone in a PhysicsAsset.
		*In component space
		*/
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(CI->ConstraintBone1, EBoneSpaces::ComponentSpace);

		/**
		*Name of second bone (body) that this constraint is connecting.
		*This will be the 'parent' bone in a PhysicsAset.
		*In component space
		*/
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(CI->ConstraintBone2, EBoneSpaces::ComponentSpace);

		//save the start rotations of all the bones
		//calculate how much bone1's rotation has changed relative to the start 
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(InitialBoneCompSpaceTransf.Find(CI->ConstraintBone1.ToString())->Rotator()));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(InitialBoneCompSpaceTransf.Find(CI->ConstraintBone2.ToString())->Rotator()));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		// Subtract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();
		
		NewFrameData.BonesData.Add(CI->ConstraintBone1.ToString(), NewBoneData);
	}

	AnimFrames.Add(NewFrameData);

	ShowMessageBox(FText::FromString("Saved current position"), FText::FromString("The current hand position was saved. Either add more frames for the grasp or press save to save all your frames in a DataAsset."));
}

// Shows a window where you can set a name for the data asset
void FMCGraspEdUtils::ShowSaveAnimationWindow()
{
	// Create a windows with two editable input boxes and a button
	TSharedRef<SWindow> SaveWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Save Grasp Animation")))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(NameTextBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Name")))
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FMCGraspEdUtils::OnSaveCallback)
		.Content()
		[
			SAssignNew(ButtonLabel, STextBlock)
			.Text(FText::FromString(TEXT("Save")))
		]

		]
		];
	IMainFrameModule& MainFrameModule =
		FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	if (MainFrameModule.GetParentWindow().IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild
		(SaveWindow, MainFrameModule.GetParentWindow()
			.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(SaveWindow);
	}
}

// Discards all currently recorded frames.
void FMCGraspEdUtils::ClearAll()
{
	// Todo reset the mesh to its original state
	//bIsInit = false;
	bInitialStateCached = false;

	InitialBoneSpaceLocations.Empty();
	InitialBoneCompSpaceTransf.Empty();

	CurrEditFrameIndex = 0;
	CurrEditGraspName = "";
	AnimFrames.Empty();

	ShowMessageBox(FText::FromString("Clear All"), FText::FromString("All your recorded frames are discarded."));
}

// Creates a message box with instructions on how to create a new grasp
void FMCGraspEdUtils::ShowNewGraspHelpWindow()
{
	FString Message = "Save Initial State:\nStores the initial state of the debug mesh (mandatory first step).\n\nAdd Frame:\nSave the current bone positions as a frame/step for the animation. You need atleast 2 frames for an animation.\n\nSave:\nSaves all frames as a DataAsset, that can then be added to a GraspController.\n\nClear All:\nDeletes all frames.\n\n";

	ShowMessageBox(FText::FromString("Help - New Grasp"), FText::FromString(Message));
}


/* Action mappings 'Edit Grasp' */
// Shows a window where you can select a frame to edit
void FMCGraspEdUtils::ShowLoadFrameWindow()
{
	// Create a windows with two editable input boxes and a button
	TSharedRef<SWindow> LoadWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Load Grasp Animation")))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(NameTextBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Name")))
		.MinDesiredWidth(200)
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(FrameNrTextBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Frame")))
		.MinDesiredWidth(200)
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FMCGraspEdUtils::OnLoadCallback)
		.Content()
		[
			SAssignNew(ButtonLabel, STextBlock)
			.Text(FText::FromString(TEXT("Load Frame")))
		]

		]
		];
	IMainFrameModule& MainFrameModule =
		FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT
		("MainFrame"));
	if (MainFrameModule.GetParentWindow().IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild
		(LoadWindow, MainFrameModule.GetParentWindow()
			.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(LoadWindow);
	}
}

// Overwrites the currently loaded step with the currently displayed bone rotations.
void FMCGraspEdUtils::UpdateFrame()
{
	if (CurrEditGraspName.IsEmpty())
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit grasping animation. You did not load any grasping animation."));
		return;
	}
	
	UMCGraspAnimDataAsset* GraspDataAssetToEdit = GetDataAsset(CurrEditGraspName);
	if (!GraspDataAssetToEdit)
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not find the grasp animation."));
	}
	

	FMCGraspAnimFrameData NewFrameData;

	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceRotations;
	int Index = 0;
	//Saves the current bone rotations as bone space. Its needed to load in a frame since only bone space is displayed in a preview.
	for (FTransform BoneTransform : BoneSpaceTransforms) 
	{
		CurrentBoneSpaceRotations.Add(BoneNames[Index].ToString(), BoneTransform.GetRotation().Rotator());
		Index++;
	}
	//go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FMCGraspAnimBoneOrientation NewBoneData = FMCGraspAnimBoneOrientation();
		//get the rotation of bones
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);
		FRotator *FoundRotation = CurrentBoneSpaceRotations.Find(NewConstraint->ConstraintBone1.ToString());
		NewBoneData.BoneSpaceRotation = *FoundRotation;

		//calculate how much bone1's roation has changed relative to the start 
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(InitialBoneCompSpaceTransf.Find(NewConstraint->ConstraintBone1.ToString())->Rotator()));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(InitialBoneCompSpaceTransf.Find(NewConstraint->ConstraintBone2.ToString())->Rotator()));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();
		NewFrameData.BonesData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	GraspDataAssetToEdit->Frames[CurrEditFrameIndex] = NewFrameData;

	// Reloads the saved step.
	// TODO Load frame could be switched with: 
	// ApplyFrame(NewFrameData);
	LoadFrame(CurrEditGraspName, CurrEditFrameIndex);

	ShowMessageBox(FText::FromString("Load Frame"), FText::FromString("The grasp was successfully edited. (TODO test this)"));
}

// Loads the next frame
void FMCGraspEdUtils::ShowNextFrame()
{
	TArray<FMCGraspAnimFrameData> Frames;
	GetFramesFromAssetName(CurrEditGraspName, Frames);

	int32 LastFrameIndex = Frames.Num() - 1;
	DebugMeshComponent->SkeletalMesh->Modify();
	int32 BoneNamesIndex = 0;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TMap<FString, FVector> BoneStartLocations;
	//Gets the current locations of all bones
	for (FTransform BoneTransform : BoneSpaceTransforms)
	{
		BoneStartLocations.Add(BoneNames[BoneNamesIndex].ToString(), BoneTransform.GetTranslation());
		BoneNamesIndex++;
	}

	if (CurrEditFrameIndex == LastFrameIndex)
	{
		CurrEditFrameIndex = 0;
	}
	else
	{
		CurrEditFrameIndex++;
	}

	ShowFrame(BoneStartLocations, Frames, CurrEditFrameIndex);
}

// Loads the previous frame
void FMCGraspEdUtils::ShowPreviousFrame()
{
	TArray<FMCGraspAnimFrameData> Frames;
	GetFramesFromAssetName(CurrEditGraspName, Frames);

	int32 LastFrameIndex = Frames.Num() - 1;
	DebugMeshComponent->SkeletalMesh->Modify();
	int32 BoneNamesIndex = 0;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TMap<FString, FVector> BoneStartLocations;
	//Gets the current locations of all bones
	for (FTransform BoneTransform : BoneSpaceTransforms)
	{
		BoneStartLocations.Add(BoneNames[BoneNamesIndex].ToString(), BoneTransform.GetTranslation());
		BoneNamesIndex++;
	}

	if (CurrEditFrameIndex == 0)
	{
		CurrEditFrameIndex = LastFrameIndex;
	}
	else
	{
		CurrEditFrameIndex--;
	}


	//Show next step.
	ShowFrame(BoneStartLocations, Frames, CurrEditFrameIndex);
}

// Creates a message box with instructions on how to create a new grasp
void FMCGraspEdUtils::ShowEditGraspHelpWindow()
{
	FString Message = "Save Initial State:\nStores the initial state of the debug mesh (mandatory first step).\n\nAdd Frame:\nSave the current bone positions as a frame/step for the animation. You need atleast 2 frames for an animation.\n\nSave:\nSaves all frames as a DataAsset, that can then be added to a GraspController.\n\nClear All:\nDeletes all frames.\n\n";

	ShowMessageBox(FText::FromString("Help - Edit Grasp"), FText::FromString(Message));
}


// Called from the save animation window, it saves the animation to the data asset
FReply FMCGraspEdUtils::OnSaveCallback()
{	
	if (NameTextBox.IsValid())
	{
		// Read the animation name from the text field
		WriteToAsset(NameTextBox->GetText().ToString());
	}
	return FReply::Handled();
}

// Called from the load frame windows, it loads a frame to the debug mesh
FReply FMCGraspEdUtils::OnLoadCallback()
{
	CurrEditGraspName = NameTextBox->GetText().ToString();
	int32 FrameNr = FCString::Atoi(*FrameNrTextBox->GetText().ToString());

	// Changes bone rotations to the given step for the given grasping style
	LoadFrame(CurrEditGraspName, FrameNr);
	return FReply::Handled();
}

// Write data to asset
void FMCGraspEdUtils::WriteToAsset(const FString& InAssetName)
{
	// Check if there are enough frames to save
	if (AnimFrames.Num() < 2)
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You need at least 2 saved frames in order to save the grasp animation.."));
		return;
	}

	// Check if the data asset already exists, or create a new one
	if (UMCGraspAnimDataAsset* ExistingDataAsset = GetDataAsset(InAssetName))
	{
		ExistingDataAsset->Name = InAssetName;
		ExistingDataAsset->Frames = AnimFrames;
	}
	else
	{
		const FString FolderPath = "/UPhysicsBasedMC/GraspAnimations/";
		const FString PackagePath =  FolderPath + InAssetName;
		UPackage* AssetPackage = CreatePackage(NULL, *PackagePath);

		UMCGraspAnimDataAsset* NewDataAsset = NewObject<UMCGraspAnimDataAsset>(
			AssetPackage->GetOutermost(), FName(*InAssetName), RF_Standalone | RF_Public);
		NewDataAsset->Name = InAssetName;
		NewDataAsset->Frames = AnimFrames;

		FAssetRegistryModule::AssetCreated(NewDataAsset);
		NewDataAsset->MarkPackageDirty();
		FString Msg = FString::Printf(TEXT("Data asset %s created in %s"), *InAssetName, *FolderPath);
		ShowMessageBox(FText::FromString("Save"), FText::FromString(Msg));

		//if (UPackage::SavePackage(AssetPackage, NewDataAsset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *InAssetName))
		//{
		//	FString Msg = FString::Printf(TEXT("Successfully saved %s to %s"), *InAssetName, *FolderPath);
		//	ShowMessageBox(FText::FromString("Save"), FText::FromString(Msg));
		//}
		//else
		//{
		//	FString Msg = FString::Printf(TEXT("Failed to save %s to %s"), *InAssetName, *FolderPath);
		//	ShowMessageBox(FText::FromString("Save"), FText::FromString(Msg));
		//}
	}
}

// Apply animation frame using the asset name and frame index
void FMCGraspEdUtils::LoadFrame(const FString& GraspAnimName, int32 FrameIndex)
{
	TArray<FMCGraspAnimFrameData> Frames;
	if (GetFramesFromAssetName(GraspAnimName, Frames))
	{
		if (Frames.IsValidIndex(FrameIndex))
		{
			DebugMeshComponent->SkeletalMesh->Modify();
			DebugMeshComponent->PreviewInstance->ResetModifiedBone();

			ApplyFrame(Frames[FrameIndex]);

			DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
		}
		else
		{
			ShowMessageBox(FText::FromString("Error"), FText::FromString("You typed in a frame that does not exists. Change the frame to edit and try again."));
			return;
		}
	}
	else
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("The chosen grasp does not exist. Make sure the value exists and try again."));
	}
}

// Shows the frame at the given index for the given HandAnimationData in the preview scene
void FMCGraspEdUtils::ShowFrame(TMap<FString, FVector> BoneStartLocations, TArray<FMCGraspAnimFrameData> Frames, int32 Index)
{
	DebugMeshComponent->SkeletalMesh->Modify();

	//Replace the current rotations with the rotations at the given step.	
	ApplyFrame(Frames[Index]);

	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

// Apply the given frame to the debug mesh
void FMCGraspEdUtils::ApplyFrame(const FMCGraspAnimFrameData& Frame)
{
	for (const auto& BoneData : Frame.BonesData)
	{
		const FRotator SpaceRotation = BoneData.Value.BoneSpaceRotation;
		int32 BoneIndex = DebugMeshComponent->GetBoneIndex(FName(*BoneData.Key));
		if (FVector* OldBoneLocation = InitialBoneSpaceLocations.Find(BoneData.Key))
		{
			DebugMeshComponent->SkeletalMesh->RetargetBasePose[BoneIndex] = FTransform(SpaceRotation, *OldBoneLocation);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find bone %s"), *FString(__func__), __LINE__, *BoneData.Key);
		}
	}
}

// Read frames from the data asset file
bool FMCGraspEdUtils::GetFramesFromAssetName(const FString& InName, TArray<FMCGraspAnimFrameData>& OutFrames)
{
	for (UMCGraspAnimDataAsset* DataAsset : GetAllDataAssets())
	{
		if (DataAsset->Name.Equals(InName))
		{
			OutFrames = DataAsset->Frames;
			return true;
		}
	}
	return false;
}

// Get data asset, returns nullptr if not found
UMCGraspAnimDataAsset* FMCGraspEdUtils::GetDataAsset(const FString& Name)
{
	for (UMCGraspAnimDataAsset* DataAsset : GetAllDataAssets())
	{
		if (DataAsset->Name.Equals(Name))
		{
			return DataAsset;
		}
	}
	return nullptr;
}

// Finds all the GraspDataAssets in a hardcoded folder
TArray<UMCGraspAnimDataAsset*> FMCGraspEdUtils::GetAllDataAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames = { TEXT("MCGraspAnimDataAsset") };
	Filter.PackagePaths.Add("/UPhysicsBasedMC/GraspAnimations");	

	TArray<FAssetData> AssetList;
	AssetRegistry.GetAssets(Filter, AssetList);

	TArray<UMCGraspAnimDataAsset*> GraspAssets;
	for (const FAssetData& DataAsset : AssetList)
	{
		UObject* Obj = DataAsset.GetAsset();
		if (Obj->GetClass()->IsChildOf(UMCGraspAnimDataAsset::StaticClass())) 
		{
			GraspAssets.Add(Cast<UMCGraspAnimDataAsset>(Obj));
		}
	}
	return GraspAssets;
}

// Creates a message dialog box
void FMCGraspEdUtils::ShowMessageBox(FText Title, FText Message)
{
	FMessageDialog* Instructions = new FMessageDialog();

	//Creates the dialog window.
	Instructions->Debugf(Message, &Title);
}

#undef LOCTEXT_NAMESPACE
