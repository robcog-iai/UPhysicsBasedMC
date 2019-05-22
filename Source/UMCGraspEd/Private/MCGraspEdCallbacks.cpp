// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCallbacks.h"
#include "Core.h"
#include "Editor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "MCGraspAnimReader.h"
#include "MCGraspEdAnimWriter.h"
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


UMCGraspEdCallbacks::UMCGraspEdCallbacks()
{
}

void UMCGraspEdCallbacks::ShowFrameEditWindow()
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
					.OnClicked_Raw(this, &UMCGraspEdCallbacks::OnEditButtonClicked)
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

void UMCGraspEdCallbacks::ShowSaveGraspAnimWindow()
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
			.OnClicked_Raw(this, &UMCGraspEdCallbacks::OnSaveButtonClicked)
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

void UMCGraspEdCallbacks::SaveBoneDatasAsFrame()
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
		FMCGraspAnimBoneData NewBoneData = FMCGraspAnimBoneData();

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

		NewFrameData.Map.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	//Checks if there is a need to create a new animation data.
	if (!bFirstCreatedFrameData)
	{
		bFirstCreatedFrameData = true;
		CreateAnimationData(CalculatedBoneRotations);
	}
	//Show an error message if the new frame could not be added.
	NewGraspAnimData.Frames.Add(NewFrameData);
	//if (!NewGraspAnimData.AddNewFrame(NewFrameData)) 
	//{

	//	ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not add a new frame. Close and open the preview scene again and repeat your last steps."));
	//}
	ShowMessageBox(FText::FromString("Saved current position"), FText::FromString("The current hand position was saved. Either add more frames for the grasp or press save to save all your frames in a DataAsset."));
}

void UMCGraspEdCallbacks::WriteFramesToAsset()
{
	//You need at least 2 frames to create a DataAsset
	if (NewGraspAnimData.Frames.Num() < 2) 
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You did not create enough frames. A grasping animation needs at least 2 frames."));
		return;
	}

	//Save all frames under the given name and reset all boolean etc.
	UMCGraspEdAnimWriter Write = UMCGraspEdAnimWriter();
	NewGraspAnimData.Name = NewGraspAnim;
	Write.WriteToDataAsset(NewGraspAnimData);
	NewGraspAnimData = FMCGraspAnimData();
	bFirstCreatedFrameData = false;

	ShowMessageBox(FText::FromString("Saved grasp"), FText::FromString("Grasp was saved as a DataAsset into the GraspingAnimations folder."));
}

void UMCGraspEdCallbacks::EditLoadedGraspAnim()
{
	if (CurrentGraspEdited.IsEmpty())
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit grasping animation. You did not load any grasping animation."));
		return;
	}
	
	FMCGraspAnimData GraspDataToEdit = UMCGraspAnimReader::ReadFile(CurrentGraspEdited);

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
		FMCGraspAnimBoneData NewBoneData = FMCGraspAnimBoneData();
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
		NewFrameData.Map.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}

	GraspDataToEdit.Frames[CurrEditFrameIndex] = NewFrameData;


	////Show an error message if the frame could not get overwritten.
	//if (!GraspDataToEdit.ReplaceFrame(CurrEditFrameIndex, NewFrameData))
	//{
	//	ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit the chosen frame."));
	//	return;
	//}

	UMCGraspEdAnimWriter::WriteToDataAsset(GraspDataToEdit);
	ShowMessageBox(FText::FromString("Edited grasp"), FText::FromString("The grasp was successfully edited."));
	//Reloads the saved step.
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrEditFrameIndex);
}

TMap<FName, FRotator> UMCGraspEdCallbacks::GetBoneRotations(USkeletalMeshComponent * SkeletalComponent)
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

void UMCGraspEdCallbacks::SetPreviewMeshComponent(UDebugSkelMeshComponent * Component)
{
	DebugMeshComponent = Component;
}

void UMCGraspEdCallbacks::CreateAnimationData(TMap<FString, FRotator> FrameData)
{
	NewGraspAnimData.BoneNames = TArray<FString>();
	for (auto& Elem : FrameData)
	{
		NewGraspAnimData.BoneNames.Add(Elem.Key);
	}
}

void UMCGraspEdCallbacks::ChangeBoneRotationsTo(FString GraspAnim, int FrameToEdit)
{
	DebugMeshComponent->SkeletalMesh->Modify();
	DebugMeshComponent->PreviewInstance->ResetModifiedBone();
	FMCGraspAnimData GraspDataToReadFrom = UMCGraspAnimReader::ReadFile(GraspAnim);

	//Show an error message if one of the parameter are wrong.
	if (GraspDataToReadFrom.Name == "") {
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

void UMCGraspEdCallbacks::ShowInstructions(FString Message)
{
	ShowMessageBox(FText::FromString("Help"), FText::FromString(Message));
}

void UMCGraspEdCallbacks::PlayOneFrame(TMap<FString, FVector> BoneStartLocations, FMCGraspAnimData PlayData, int Index)
{
	DebugMeshComponent->SkeletalMesh->Modify();

	//Replace the current rotations with the rotations at the given step.
	ApplyBoneDataForIndex(PlayData, Index);

	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

void UMCGraspEdCallbacks::DiscardAllFrames()
{
	NewGraspAnimData = FMCGraspAnimData();
	Reset();
	ShowMessageBox(FText::FromString("Discard successful"), FText::FromString("All your recorded frames are discarded."));
}

void UMCGraspEdCallbacks::ShowFrame(bool bForward)
{
	FMCGraspAnimData HandAnimationData = UMCGraspAnimReader::ReadFile(CurrentGraspEdited);
	int MaxFrames = HandAnimationData.Frames.Num() - 1;
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
		if (CurrEditFrameIndex == MaxFrames)
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
			CurrEditFrameIndex = MaxFrames;
		}
		else 
		{
			CurrEditFrameIndex -= 1;
		}
	}

	//Show next step.
	PlayOneFrame(BoneStartLocations, HandAnimationData, CurrEditFrameIndex);
}

void UMCGraspEdCallbacks::Reset()
{
	bSavedStartTransforms = false;
	StartBoneLocBoneSpace.Empty();
	StartBoneTransCompSpace.Empty();
	CurrEditFrameIndex = 0;
	CurrentGraspEdited = "";
	NewGraspAnim = "";
	bFirstCreatedFrameData = false;
}

void UMCGraspEdCallbacks::ShowMessageBox(FText Title, FText Message)
{
	FMessageDialog* Instructions = new FMessageDialog();

	//Creates the dialog window.
	Instructions->Debugf(Message,&Title);
}

void UMCGraspEdCallbacks::SaveStartTransforms()
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

void UMCGraspEdCallbacks::ApplyBoneDataForIndex(const FMCGraspAnimData& Anim, int32 Index)
{
	FMCGraspAnimFrameData AnimFrame = Anim.Frames[Index];

	//Apply the rotations at a given step for the given AnimationData on the DebugMeshComponent
	for (auto BoneDataEntry : AnimFrame.Map)
	{
		FRotator BoneData = BoneDataEntry.Value.BoneSpaceRotation;
		FRotator SwitchYawPitch = FRotator(BoneData.Pitch, BoneData.Yaw, BoneData.Roll);
		int32 BoneIndex = DebugMeshComponent->GetBoneIndex(FName(*BoneDataEntry.Key));
		FVector* OldBoneLocation = StartBoneLocBoneSpace.Find(BoneDataEntry.Key);
		DebugMeshComponent->SkeletalMesh->RetargetBasePose[BoneIndex] = FTransform(BoneData, *OldBoneLocation);
	}
}

FReply UMCGraspEdCallbacks::OnEditButtonClicked()
{
	CurrentGraspEdited = GraspAnimBox->GetText().ToString();
	FText FrameToEdit = FrameBox->GetText();
	CurrEditFrameIndex = FCString::Atoi(*FrameToEdit.ToString());

	//Changes bone rotations to the given step for the given grasping stlye
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrEditFrameIndex);
	return FReply::Handled();
}

FReply UMCGraspEdCallbacks::OnSaveButtonClicked()
{
	NewGraspAnim = NewGraspAnimNameBox->GetText().ToString();

	//Saves a new grasp anim under the name given name
	WriteFramesToAsset();
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE