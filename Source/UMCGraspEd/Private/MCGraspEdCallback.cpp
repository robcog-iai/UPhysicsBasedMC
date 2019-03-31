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
	if (!bSavedStartTransforms) 
	{
		return;
	}

	TMap<FString, FMCBoneData> NewFrameData = TMap<FString, FMCBoneData>();
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
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(StartBoneTransCompSpace.Find(NewConstraint->ConstraintBone1.ToString())->Rotator()));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(StartBoneTransCompSpace.Find(NewConstraint->ConstraintBone2.ToString())->Rotator()));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularOrientationTarget = Quat.Rotator();

		CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), (Quat.Rotator()));

		NewFrameData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	//Checks if there is a need to create a new animation data.
	if (!bFirstCreatedFrameData)
	{
		bFirstCreatedFrameData = true;
		CreateAnimationData(CalculatedBoneRotations);
	}
	//Show an error message if the new frame could not be added.
	if (!NewGraspAnimData.AddNewFrame(NewFrameData)) {

		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not add a new frame. Close and open the preview scene again and repeat your last steps."));
	}
	ShowMessageBox(FText::FromString("Saved current position"), FText::FromString("The current hand position was saved. Either add more frames for the grasp or press save to save all your frames in a DataAsset."));
}

void UMCGraspEdCallback::WriteFramesToAsset()
{
	//You need at least 2 frames to create a DataAsset
	if (NewGraspAnimData.GetNumberOfFrames() < 2) 
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You did not create enough frames. A grasping animation needs at least 2 frames."));
		return;
	}

	//Save all frames under the given name and reset all boolean etc.
	UMCGraspAnimWriter Write = UMCGraspAnimWriter();
	NewGraspAnimData.AnimationName = NewGraspAnim;
	Write.WriteFile(NewGraspAnimData);
	NewGraspAnimData = FMCAnimationData();
	bFirstCreatedFrameData = false;

	ShowMessageBox(FText::FromString("Saved grasp"), FText::FromString("Grasp was saved as a DataAsset into the GraspingAnimations folder."));
}

void UMCGraspEdCallback::EditLoadedGraspAnim()
{
	if (CurrentGraspEdited.IsEmpty())
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit grasping animation. You did not load any grasping animation."));
		return;
	}
	
	FMCAnimationData GraspDataToEdit = UMCGraspAnimReader::ReadFile(CurrentGraspEdited);

	TMap<FString, FMCBoneData> NewFrameData = TMap<FString, FMCBoneData>();

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
		FMCBoneData NewBoneData = FMCBoneData();
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
		NewFrameData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	//Show an error message if the frame could not get overwritten.
	if (!GraspDataToEdit.ReplaceFrame(CurrentEditedFrame, NewFrameData))
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

void UMCGraspEdCallback::CreateAnimationData(TMap<FString, FRotator> FrameData)
{
	NewGraspAnimData.BoneNames = TArray<FString>();
	for (auto& Elem : FrameData)
	{
		NewGraspAnimData.BoneNames.Add(Elem.Key);
	}
}

void UMCGraspEdCallback::ChangeBoneRotationsTo(FString GraspAnim, int FrameToEdit)
{
	DebugMeshComponent->SkeletalMesh->Modify();
	DebugMeshComponent->PreviewInstance->ResetModifiedBone();
	FMCAnimationData GraspDataToReadFrom = UMCGraspAnimReader::ReadFile(GraspAnim);

	//Show an error message if one of the parameter are wrong.
	if (GraspDataToReadFrom.AnimationName == "") {
		ShowMessageBox(FText::FromString("Error"), FText::FromString("The chosen grasp does not exist. Make sure the value exists and try again."));
		return;
	}

	if (FrameToEdit < 0) {
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You typed in a frame that does not exists. Change the frame to edit and try again."));
		return;
	}
	
	//Replace the current rotations with the rotations at the given step.
	ApplyBoneDataForIndex(GraspDataToReadFrom, FrameToEdit);
	
	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

void UMCGraspEdCallback::ShowInstructions(FString Message)
{
	ShowMessageBox(FText::FromString("Help"), FText::FromString(Message));
}

void UMCGraspEdCallback::PlayOneFrame(TMap<FString, FVector> BoneStartLocations, FMCAnimationData PlayData, int Index)
{
	DebugMeshComponent->SkeletalMesh->Modify();

	//Replace the current rotations with the rotations at the given step.
	ApplyBoneDataForIndex(PlayData, Index);

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
	FMCAnimationData HandAnimationData = UMCGraspAnimReader::ReadFile(CurrentGraspEdited);
	int MaxFrames = HandAnimationData.GetNumberOfFrames() - 1;
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
		if (CurrentEditedFrame == MaxFrames)
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
			CurrentEditedFrame = MaxFrames;
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
	bSavedStartTransforms = false;
	StartBoneLocBoneSpace.Empty();
	StartBoneTransCompSpace.Empty();
	CurrentEditedFrame = 0;
	CurrentGraspEdited = "";
	NewGraspAnim = "";
	bFirstCreatedFrameData = false;
}

void UMCGraspEdCallback::ShowMessageBox(FText Title, FText Message)
{
	FMessageDialog* Instructions = new FMessageDialog();

	//Creates the dialog window.
	Instructions->Debugf(Message,&Title);
}

void UMCGraspEdCallback::SaveStartTransforms()
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

void UMCGraspEdCallback::ApplyBoneDataForIndex(FMCAnimationData PlayData, int Index)
{
	FMCFrame FrameData = PlayData.GetPositionDataWithIndex(Index);
	TMap<FString, FMCBoneData>* FrameDataMap = FrameData.GetMap();

	//Apply the rotations at a given step for the given AnimationData on the DebugMeshComponent
	for (auto BoneDataEntry : *FrameDataMap)
	{
		FRotator BoneData = BoneDataEntry.Value.BoneSpaceRotation;
		FRotator SwitchYawPitch = FRotator(BoneData.Pitch, BoneData.Yaw, BoneData.Roll);
		int BoneIndex = DebugMeshComponent->GetBoneIndex(FName(*BoneDataEntry.Key));
		FVector* OldBoneLocation = StartBoneLocBoneSpace.Find(BoneDataEntry.Key);
		DebugMeshComponent->SkeletalMesh->RetargetBasePose[BoneIndex] = FTransform(BoneData, *OldBoneLocation);
	}
}

FReply UMCGraspEdCallback::OnEditButtonClicked()
{
	CurrentGraspEdited = GraspAnimBox->GetText().ToString();
	FText FrameToEdit = FrameBox->GetText();
	CurrentEditedFrame = FCString::Atoi(*FrameToEdit.ToString());

	//Changes bone rotations to the given step for the given grasping stlye
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrentEditedFrame);
	return FReply::Handled();
}

FReply UMCGraspEdCallback::OnSaveButtonClicked()
{
	NewGraspAnim = NewGraspAnimNameBox->GetText().ToString();

	//Saves a new grasp anim under the name given name
	WriteFramesToAsset();
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE