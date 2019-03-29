// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCallback.h"
#include "Core.h"
#include "Editor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "MCGraspAnimReader.h"
#include "MCGraspAnimWriter.h"
#include "MCGraspAnimStructs.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableText.h"
#include "Editor/AnimGraph/Classes/AnimPreviewInstance.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include <thread>   

#define LOCTEXT_NAMESPACE "FMCGraspingEditorModule"


UMCGraspEdCallback::UMCGraspEdCallback()
{
}

void UMCGraspEdCallback::ShowFrameEditWindow()
{
	//Creates the edit menu with 2 editable textfields and a button
	TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Edit Grasp Anima")))
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
					.OnClicked_Raw(this, &UMCGraspEdCallback::OnEditButtonClicked)
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

void UMCGraspEdCallback::ShowSaveGraspAnimWindow()
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
			.OnClicked_Raw(this, &UMCGraspEdCallback::OnSaveButtonClicked)
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

void UMCGraspEdCallback::SaveBoneDatasAsFrame()
{
	if (!StartRotatorsSet) 
	{
		return;
	}

	TMap<FString, FMCBoneData> NewEpisodeData = TMap<FString, FMCBoneData>();
	TMap<FString, FRotator> CalculatedBoneRotations;

	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceRotations;
	int Index = 0;

	//Saves the current bone rotations as bone space. Its needed to load in an episode since only bone space is displayed in a preview.
	for (FTransform BoneTransform : BoneSpaceTransforms) 
	{
		CurrentBoneSpaceRotations.Add(BoneNames[Index].ToString(), BoneTransform.GetRotation().Rotator());
		Index++;
	}

	//go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FMCBoneData NewBoneData = FMCBoneData();

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
		//calculate how much bone1's roation has changed relative to the start 
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(*StartRotatorsComponentSpace.Find(NewConstraint->ConstraintBone1.ToString())));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(*StartRotatorsComponentSpace.Find(NewConstraint->ConstraintBone2.ToString())));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();

		CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), (Quat.Rotator()));

		NewEpisodeData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	//Checks if there is a need to create a new animation data.
	if (!bFirstCreatedFrameData)
	{
		bFirstCreatedFrameData = true;
		CreateAnimationData(CalculatedBoneRotations);
	}
	//Show an error message if the new episode could not be added.
	if (!NewGraspAnimData.AddNewEpisode(NewEpisodeData)) {

		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not add a new frame. Close and open the preview scene again and repeat your last steps."));
	}
	ShowMessageBox(FText::FromString("Saved current position"), FText::FromString("The current hand position was saved. Either add more frames for the grasp or press save to save all your episodes in a DataAsset."));
}

void UMCGraspEdCallback::WriteFramesToAsset()
{
	//You need at least 2 episodes to create a 
	if (NewGraspAnimData.GetNumberOfEpisodes() < 2) 
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You did not create enough frames. A grasping animation needs at least 2 frames."));
		return;
	}

	//Save all episodes under the given name and reset all boolean etc.
	UMCGraspAnimWriter Write = UMCGraspAnimWriter();
	NewGraspAnimData.AnimationName = NewGraspAnim;
	Write.WriteFile(NewGraspAnimData);
	NewGraspAnimData = FMCAnimationData();
	bFirstCreatedFrameData = false;

	ShowMessageBox(FText::FromString("Saved grasp"), FText::FromString("Grasp was saved as a DataAsset into the GraspingAnimations folder."));
}

void UMCGraspEdCallback::EditLoadedGraspAnim()
{
	if (CurrentGraspEdited.IsEmpty() || CurrentEditedFrame == 0)
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit grasping animation. Either you did not load any grasping animation or you are trying to edit the frame 0."));
		return;
	}
	
	FMCAnimationData GraspDataToEdit = UMCGraspAnimReader::ReadFile(DebugMeshComponent->SkeletalMesh->GetFName().ToString(), CurrentGraspEdited);

	TMap<FString, FMCBoneData> NewEpisodeData = TMap<FString, FMCBoneData>();

	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceRotations;
	int Index = 0;
	//Saves the current bone rotations as bone space. Its needed to load in an episode since only bone space is displayed in a preview.
	for (FTransform BoneTransform : BoneSpaceTransforms) 
	{
		CurrentBoneSpaceRotations.Add(BoneNames[Index].ToString(), BoneTransform.GetRotation().Rotator());
		Index++;
	}
	//go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FMCBoneData NewBoneData = FMCBoneData();
		//get the rotation of bones
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);
		FRotator *FoundRotation = CurrentBoneSpaceRotations.Find(NewConstraint->ConstraintBone1.ToString());
		NewBoneData.BoneSpaceRotation = *FoundRotation;

		//calculate how much bone1's roation has changed relative to the start 
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(*StartRotatorsComponentSpace.Find(NewConstraint->ConstraintBone1.ToString())));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(*StartRotatorsComponentSpace.Find(NewConstraint->ConstraintBone2.ToString())));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();
		NewEpisodeData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	//Show an error message if the episode could not get overwritten.
	if (!GraspDataToEdit.ReplaceEpisode(CurrentEditedFrame, NewEpisodeData))
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit the chosen frame."));
		return;
	}
	UMCGraspAnimWriter::WriteFile(GraspDataToEdit);
	ShowMessageBox(FText::FromString("Edited grasp"), FText::FromString("The grasp was successfully edited."));
	//Reloads the saved step.
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrentEditedFrame);
}

TMap<FName, FRotator> UMCGraspEdCallback::GetBoneRotations(USkeletalMeshComponent * SkeletalComponent)
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

void UMCGraspEdCallback::SetPreviewMeshComponent(UDebugSkelMeshComponent * Component)
{
	DebugMeshComponent = Component;
}

void UMCGraspEdCallback::CreateAnimationData(TMap<FString, FRotator> EpisodeData)
{
	NewGraspAnimData.BoneNames = TArray<FString>();
	for (auto& Elem : EpisodeData)
	{
		NewGraspAnimData.BoneNames.Add(Elem.Key);
	}
}

void UMCGraspEdCallback::ChangeBoneRotationsTo(FString GraspingStyle, int EpisodeToEdit)
{
	DebugMeshComponent->SkeletalMesh->Modify();
	DebugMeshComponent->PreviewInstance->ResetModifiedBone();
	FMCAnimationData GraspDataToReadFrom = UMCGraspAnimReader::ReadFile(DebugMeshComponent->SkeletalMesh->GetFName().ToString(), GraspingStyle);

	//Show an error message if one of the parameter are wrong.
	if (GraspDataToReadFrom.AnimationName == "") {
		ShowMessageBox(FText::FromString("Error"), FText::FromString("The chosen grasp does not exist. Make sure the value exists and try again."));
		return;
	}

	if (EpisodeToEdit < 0) {
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You typed in a frame that does not exists. Change the frame to edit and try again."));
		return;
	}

	//Gets the current locations for the displayed hand
	FillStartLocations();

	//Reset to starting rotations.
	ApplyFingerDataForStartingIndex(GraspDataToReadFrom);
	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
	
	//Gets starting rotations in component space.
	FillStartingRotatorsInComponentSpace();
	
	//Replace the current rotations with the rotations at the given step.
	ApplyFingerDataForIndex(GraspDataToReadFrom, EpisodeToEdit);
	
	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

void UMCGraspEdCallback::ShowInstructions(FString Message)
{
	ShowMessageBox(FText::FromString("Help"), FText::FromString(Message));
}

void UMCGraspEdCallback::PlayOneFrame(TMap<FString, FVector> BoneStartLocations, FMCAnimationData PlayData, int Index)
{
	DebugMeshComponent->SkeletalMesh->Modify();

	//Replace the current rotations with the rotations at step 0 for the currently loaded grasping stlye.
	ApplyFingerDataForStartingIndex(PlayData);
	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);

	//Replace the current rotations with the rotations at the given step.
	ApplyFingerDataForIndex(PlayData, Index);

	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

void UMCGraspEdCallback::DiscardAllFrames()
{
	NewGraspAnimData = FMCAnimationData();
	Reset();
	ShowMessageBox(FText::FromString("Discard successful"), FText::FromString("All your recorded frames are discarded."));
}

void UMCGraspEdCallback::ShowFrame(bool bForward)
{
	FMCAnimationData HandAnimationData = UMCGraspAnimReader::ReadFile(DebugMeshComponent->SkeletalMesh->GetFName().ToString(), CurrentGraspEdited);
	int MaxEpisode = HandAnimationData.GetNumberOfEpisodes() - 1;
	DebugMeshComponent->SkeletalMesh->Modify();
	int BoneNamesIndex = 0;
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
		if (CurrentEditedFrame == MaxEpisode)
		{
			CurrentEditedFrame = 0;
		}
		else 
		{
			CurrentEditedFrame += 1;
		}
	}
	else 
	{
		if (CurrentEditedFrame == 0)
		{
			CurrentEditedFrame = MaxEpisode;
		}
		else 
		{
			CurrentEditedFrame -= 1;
		}
	}

	//Show next step.
	PlayOneFrame(BoneStartLocations, HandAnimationData, CurrentEditedFrame);
}

void UMCGraspEdCallback::Reset()
{
	StartRotatorsSet = false;
	StartBoneLocations.Empty();
	StartRotatorsComponentSpace.Empty();
	StartBoneTransforms.Empty();
	CurrentEditedFrame = 0;
	CurrentGraspEdited = "";
	NewGraspAnim = "";
	bFirstCreatedFrameData = false;
}

void UMCGraspEdCallback::SetStartingBoneTransforms(TMap<FString, FTransform> BoneRotations)
{
	StartBoneTransforms = BoneRotations;
}

void UMCGraspEdCallback::ShowMessageBox(FText Title, FText Message)
{
	FMessageDialog* Instructions = new FMessageDialog();

	//Creates the dialog window.
	Instructions->Debugf(Message,&Title);
}

void UMCGraspEdCallback::FillStartingRotatorsInComponentSpace()
{
	//Gets all rotations of the bones in components space.
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);

		StartRotatorsComponentSpace.Add(NewConstraint->ConstraintBone1.ToString(), QuatBone1);
		StartRotatorsComponentSpace.Add(NewConstraint->ConstraintBone2.ToString(), QuatBone2);
	}
	StartRotatorsSet = true;
}

void UMCGraspEdCallback::ApplyFingerDataForStartingIndex(FMCAnimationData PlayData)
{
	FMCFrame FingerDataStartingIndex = PlayData.GetPositionDataWithIndex(0);
	TMap<FString, FMCBoneData>* FingerDataMapStartingIndex = FingerDataStartingIndex.GetMap();

	//Apply the rotations at step 0 for the given AnimationData on the DebugMeshComponent
	for (auto BoneDataEntry : *FingerDataMapStartingIndex)
	{
		FRotator BoneData = BoneDataEntry.Value.BoneSpaceRotation;
		FRotator SwitchYawPitch = FRotator(BoneData.Pitch, BoneData.Yaw, BoneData.Roll);
		int Index = DebugMeshComponent->GetBoneIndex(FName(*BoneDataEntry.Key));
		FTransform* OldBoneTransform = StartBoneTransforms.Find(BoneDataEntry.Key);
		FVector* OldBoneLocation = StartBoneLocations.Find(BoneDataEntry.Key);
		DebugMeshComponent->SkeletalMesh->RetargetBasePose[Index] = FTransform(BoneData, *OldBoneLocation);
	}
}

void UMCGraspEdCallback::ApplyFingerDataForIndex(FMCAnimationData PlayData, int Index)
{
	FMCFrame FingerData = PlayData.GetPositionDataWithIndex(Index);
	TMap<FString, FMCBoneData>* FingerDataMap = FingerData.GetMap();

	//Apply the rotations at a given step for the given AnimationData on the DebugMeshComponent
	for (auto BoneDataEntry : *FingerDataMap)
	{
		FRotator BoneData = BoneDataEntry.Value.BoneSpaceRotation;
		FRotator SwitchYawPitch = FRotator(BoneData.Pitch, BoneData.Yaw, BoneData.Roll);
		int BoneIndex = DebugMeshComponent->GetBoneIndex(FName(*BoneDataEntry.Key));
		FTransform OldBoneTransform = DebugMeshComponent->GetBoneTransform(BoneIndex);
		FVector* OldBoneLocation = StartBoneLocations.Find(BoneDataEntry.Key);
		DebugMeshComponent->SkeletalMesh->RetargetBasePose[BoneIndex] = FTransform(BoneData, *OldBoneLocation);
	}
}

void UMCGraspEdCallback::FillStartLocations()
{
	//Gets the current starting locations if the DebugMeshComponent
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	int Index = 0;
	for (FTransform BoneTransform : DebugMeshComponent->BoneSpaceTransforms) 
	{
		StartBoneLocations.Add(BoneNames[Index].ToString(), BoneTransform.GetTranslation());
		Index++;
	}
}

FReply UMCGraspEdCallback::OnEditButtonClicked()
{
	CurrentGraspEdited = GraspAnimBox->GetText().ToString();
	FText EpisodeToEdit = FrameBox->GetText();
	CurrentEditedFrame = FCString::Atoi(*EpisodeToEdit.ToString());

	//Changes bone rotations to the given step for the given grasping stlye
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrentEditedFrame);
	return FReply::Handled();
}

FReply UMCGraspEdCallback::OnSaveButtonClicked()
{
	NewGraspAnim = NewGraspAnimNameBox->GetText().ToString();

	//Saves a new grasping style under the name given name
	WriteFramesToAsset();
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE