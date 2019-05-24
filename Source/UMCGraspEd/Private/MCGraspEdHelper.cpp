// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdHelper.h"
#include "Core.h"
#include "Editor.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "Widgets/Input/SEditableText.h"
#include "AnimPreviewInstance.h"
#include "Interfaces/IMainFrameModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "FMCGraspEdModule"

// Default ctor
FMCGraspEdHelper::FMCGraspEdHelper()
{
	bFirstCreatedFrameData = false;
	bSavedStartTransforms = false;
	bContinuePlayGrasp = true;
	bIsInit = false;
	CurrEditFrameIndex = 0;
	NewGraspAnimName = "";
	CurrGraspName = "";
}

// Init the helper functions
void FMCGraspEdHelper::Init(UDebugSkelMeshComponent* InDebugMeshComponent)
{
	DebugMeshComponent = InDebugMeshComponent;
	if (DebugMeshComponent)
	{
		bIsInit = true;
	}
}

// Saves the current position of the displayed mesh to a new frame
void FMCGraspEdHelper::SaveToFrame()
{
	if (!bSavedStartTransforms) 
	{
		return;
	}

	FMCGraspAnimFrameData NewFrameData;
	TMap<FString, FRotator> CalculatedBoneRotations;

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

		// Gets BoneSpaceRotation from CurrentBoneSpaceRotations
		FRotator *InBoneSpaceRotation = CurrentBoneSpaceRotations.Find(NewConstraint->ConstraintBone1.ToString());
		NewBoneData.BoneSpaceRotation = *InBoneSpaceRotation;

		/**
		*Name of first bone(body) that this constraint is connecting.
		*This will be the 'child' bone in a PhysicsAsset.
		*In component space
		*/
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);

		/**
		*Name of second bone (body) that this constraint is connecting.
		*This will be the 'parent' bone in a PhysicsAset.
		*In component space
		*/
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);

		//save the start rotations of all the bones
		//calculate how much bone1's rotation has changed relative to the start 
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(StartBoneTransCompSpace.Find(NewConstraint->ConstraintBone1.ToString())->Rotator()));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(StartBoneTransCompSpace.Find(NewConstraint->ConstraintBone2.ToString())->Rotator()));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();

		CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), (Quat.Rotator()));

		NewFrameData.BonesData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	//Checks if there is a need to create a new animation data.
	if (!bFirstCreatedFrameData)
	{
		bFirstCreatedFrameData = true;
	}
	//Show an error message if the new frame could not be added.
	//NewGraspAnimData.Frames.Add(NewFrameData);
	NewFrames.Add(NewFrameData);

	ShowMessageBox(FText::FromString("Saved current position"), FText::FromString("The current hand position was saved. Either add more frames for the grasp or press save to save all your frames in a DataAsset."));
}

// Writes all currently recorded frames to the data asset.
void FMCGraspEdHelper::WriteFramesToAsset()
{
	//You need at least 2 frames to create a DataAsset	
	if (NewFrames.Num() < 2)
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You did not create enough frames. A grasping animation needs at least 2 frames."));
		return;
	}

	//Save all frames under the given name and reset all boolean etc.
	
	WriteToDataAsset(NewGraspAnimName, NewFrames);
	NewFrames.Empty();
	bFirstCreatedFrameData = false;

	ShowMessageBox(FText::FromString("Saved grasp"), FText::FromString("Grasp was saved as a DataAsset into the GraspingAnimations folder."));
}

// Overwrites the currently loaded step with the currently displayed bone rotations.
void FMCGraspEdHelper::EditLoadedGraspAnim()
{
	if (CurrGraspName.IsEmpty())
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit grasping animation. You did not load any grasping animation."));
		return;
	}
	
	UMCGraspAnimDataAsset* GraspDataAssetToEdit = GetAnimGraspDataAsset(CurrGraspName);
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
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(StartBoneTransCompSpace.Find(NewConstraint->ConstraintBone1.ToString())->Rotator()));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(StartBoneTransCompSpace.Find(NewConstraint->ConstraintBone2.ToString())->Rotator()));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();
		NewFrameData.BonesData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	GraspDataAssetToEdit->Frames[CurrEditFrameIndex] = NewFrameData;

	ShowMessageBox(FText::FromString("Edited grasp"), FText::FromString("The grasp was successfully edited. (TODO test this)"));
	//Reloads the saved step.
	ChangeBoneRotationsTo(CurrGraspName, CurrEditFrameIndex);
}

// Loads a step from a grasp anim
void FMCGraspEdHelper::ChangeBoneRotationsTo(const FString& GraspAnimName, int32 FrameIndex)
{
	TArray<FMCGraspAnimFrameData> Frames;
	if (ReadFramesFromName(GraspAnimName, Frames))
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

// Shows a help windows with the given message.	
void FMCGraspEdHelper::ShowInstructions(FString Message)
{
	ShowMessageBox(FText::FromString("Help"), FText::FromString(Message));
}

// Shows the frame at the given index for the given HandAnimationData in the preview scene
void FMCGraspEdHelper::PlayOneFrame(TMap<FString, FVector> BoneStartLocations, TArray<FMCGraspAnimFrameData> Frames, int32 Index)
{
	DebugMeshComponent->SkeletalMesh->Modify();

	//Replace the current rotations with the rotations at the given step.	
	ApplyFrame(Frames[Index]);

	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

// Discards all currently recorded frames.
void FMCGraspEdHelper::DiscardAllFrames()
{
	NewGraspAnimName = "";
	NewFrames.Empty();
	Reset();
	ShowMessageBox(FText::FromString("Discard successful"), FText::FromString("All your recorded frames are discarded."));
}

// Shows a frame. bForward true if the next frame should get shown, false for the previous frame.
void FMCGraspEdHelper::ShowFrame(bool bForward)
{
	TArray<FMCGraspAnimFrameData> Frames;
	ReadFramesFromName(CurrGraspName, Frames);

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

	//Determines the next step to show
	if (bForward) 
	{
		if (CurrEditFrameIndex == LastFrameIndex)
		{
			CurrEditFrameIndex = 0;
		}
		else 
		{
			CurrEditFrameIndex += 1;
		}
	}
	else 
	{
		if (CurrEditFrameIndex == 0)
		{
			CurrEditFrameIndex = LastFrameIndex;
		}
		else 
		{
			CurrEditFrameIndex -= 1;
		}
	}

	//Show next step.
	PlayOneFrame(BoneStartLocations, Frames, CurrEditFrameIndex);
}

// Resets all variables used in this Class.
void FMCGraspEdHelper::Reset()
{
	bIsInit = false;
	bSavedStartTransforms = false;
	StartBoneLocBoneSpace.Empty();
	StartBoneTransCompSpace.Empty();
	CurrEditFrameIndex = 0;
	CurrGraspName = "";
	NewGraspAnimName = "";
	bFirstCreatedFrameData = false;
}


// Saves all the transforms we need later on, before bones are moved
void FMCGraspEdHelper::SaveStartTransforms()
{
	//Gets all locations of the bones in bone space
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	int Index = 0;
	for (FTransform BoneTransform : DebugMeshComponent->BoneSpaceTransforms)
	{
		StartBoneLocBoneSpace.Add(BoneNames[Index].ToString(), BoneTransform.GetTranslation());
		Index++;
	}

	//Gets all the transforms of the bones in component space
	for (FName BoneName : BoneNames)
	{
		int Index = DebugMeshComponent->GetBoneIndex(BoneName);
		FTransform BoneTransform = DebugMeshComponent->GetBoneTransform(Index);
		StartBoneTransCompSpace.Add(BoneName.ToString(), BoneTransform);
	}

	bSavedStartTransforms = true;
}

// Creates a message dialog box
void FMCGraspEdHelper::ShowMessageBox(FText Title, FText Message)
{
	FMessageDialog* Instructions = new FMessageDialog();

	//Creates the dialog window.
	Instructions->Debugf(Message,&Title);
}

// Apply the given frame to the debug mesh
void FMCGraspEdHelper::ApplyFrame(const FMCGraspAnimFrameData& Frame)
{
	for (const auto& BoneData : Frame.BonesData)
	{
		const FRotator SpaceRotation = BoneData.Value.BoneSpaceRotation;
		int32 BoneIndex = DebugMeshComponent->GetBoneIndex(FName(*BoneData.Key));
		if (FVector* OldBoneLocation = StartBoneLocBoneSpace.Find(BoneData.Key))
		{
			DebugMeshComponent->SkeletalMesh->RetargetBasePose[BoneIndex] = FTransform(SpaceRotation, *OldBoneLocation);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find bone %s"), *FString(__func__), __LINE__, *BoneData.Key);
		}
	}
}

// Get data asset, returns nullptr if not found
UMCGraspAnimDataAsset* FMCGraspEdHelper::GetAnimGraspDataAsset(const FString& Name)
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
bool FMCGraspEdHelper::ReadFramesFromName(const FString& Name, TArray<FMCGraspAnimFrameData>& OutFrames)
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

// Returns all GraspDataAssets that can be read
TArray<UMCGraspAnimDataAsset*> FMCGraspEdHelper::LoadAllAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	while (AssetRegistry.IsLoadingAssets())
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Loading.."), *FString(__func__), __LINE__);
	}

	return OnRegistryLoaded();
}

// Finds all the GraspDataAssets in a hardcoded folder
TArray<UMCGraspAnimDataAsset*> FMCGraspEdHelper::OnRegistryLoaded()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames = { TEXT("MCGraspAnimDataAsset") };
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

// Create and save data asset
void FMCGraspEdHelper::WriteToDataAsset(const FString& InAnimName, const TArray<FMCGraspAnimFrameData>& InFrames)
{
	if (UMCGraspAnimDataAsset* DataAsset = GetAnimGraspDataAsset(InAnimName))
	{
		DataAsset->Name = InAnimName;
		DataAsset->Frames = InFrames;
	}
	else
	{
		UMCGraspEdAnimDataAssetFactory* GraspAnimFactory = NewObject<UMCGraspEdAnimDataAssetFactory>();
		GraspAnimFactory->AddGraspDataAsset(InAnimName, InFrames);
	}
}


FReply FMCGraspEdHelper::OnEditButtonClicked()
{
	CurrGraspName = GraspAnimBox->GetText().ToString();
	FText FrameToEdit = FrameBox->GetText();
	CurrEditFrameIndex = FCString::Atoi(*FrameToEdit.ToString());

	//Changes bone rotations to the given step for the given grasping stlye
	ChangeBoneRotationsTo(CurrGraspName, CurrEditFrameIndex);
	return FReply::Handled();
}

FReply FMCGraspEdHelper::OnSaveButtonClicked()
{
	NewGraspAnimName = NewGraspAnimNameBox->GetText().ToString();

	//Saves a new grasp anim under the name given name
	WriteFramesToAsset();
	return FReply::Handled();
}


TMap<FName, FRotator> FMCGraspEdHelper::GetBoneRotations(USkeletalMeshComponent * SkeletalComponent)
{
	TMap<FName, FRotator> BoneRotations;
	TArray<FName> BoneNames;
	SkeletalComponent->GetBoneNames(BoneNames);
	//Gets the rotations of every bone in the given mesh and saves them in a map with their bone name as key.
	for (FName BoneName : BoneNames)
	{
		int index = SkeletalComponent->GetBoneIndex(BoneName);
		FTransform Transform = SkeletalComponent->GetBoneTransform(index);
		FQuat Quat = Transform.GetRotation();
		BoneRotations.Add(BoneName, Quat.Rotator());
	}
	return BoneRotations;
}


// Shows a window where you can select a frame to edit
void FMCGraspEdHelper::ShowFrameEditWindow()
{
	//Creates the edit menu with 2 editable textfields and a button
	TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Edit Grasp Animation")))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(GraspAnimBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Name")))
		.MinDesiredWidth(200)
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(FrameBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Frame")))
		.MinDesiredWidth(200)
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FMCGraspEdHelper::OnEditButtonClicked)
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
		(CookbookWindow, MainFrameModule.GetParentWindow()
			.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(CookbookWindow);
	}
}

// Shows a window where you can set a name for the data asset
void FMCGraspEdHelper::ShowSaveGraspAnimWindow()
{
	//Creates the save menu with 2 editable textfields and a button
	TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Save")))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(NewGraspAnimNameBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Name")))
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.OnClicked_Raw(this, &FMCGraspEdHelper::OnSaveButtonClicked)
		.Content()
		[
			SAssignNew(ButtonLabel, STextBlock)
			.Text(FText::FromString(TEXT("Save")))
		]

		]
		];
	IMainFrameModule& MainFrameModule =
		FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT
		("MainFrame"));
	if (MainFrameModule.GetParentWindow().IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild
		(CookbookWindow, MainFrameModule.GetParentWindow()
			.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(CookbookWindow);
	}
}

void FMCGraspEdHelper::ShowCreateHelp()
{
	FString Message = "Start:\nStarts the process of creating a new grasping animation. Always press this first.\n\nAdd Frame:\nSave the current bone positions as a frame/step for the animation. You need atleast 2 frames for an animation.\n\nSave:\nSaves all frames as a DataAsset, that can then be added to a GraspController.\n\nClear All:\nDeletes all frames.\n\n";
	ShowInstructions(Message);
}

#undef LOCTEXT_NAMESPACE