// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspingEditorCallback.h"
#include "Core.h"
#include "Editor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "MCRead.h"
#include "MCWrite.h"
#include "MCAnimationDataStructure.h"
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


UMCGraspingEditorCallback::UMCGraspingEditorCallback()
{
}

void UMCGraspingEditorCallback::ShowEpisodeEditWindow()
{
	//Creates the edit menu with 2 editable textfields and a button
	TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Edit grasp")))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SAssignNew(GraspingStyleBox, SEditableTextBox)
				.Text(FText::FromString(TEXT("Enter grasping style to edit")))
				.MinDesiredWidth(200)
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SAssignNew(EpisodeBox, SEditableTextBox)
					.Text(FText::FromString(TEXT("Enter specific episode to edit")))
					.MinDesiredWidth(200)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.OnClicked_Raw(this, &UMCGraspingEditorCallback::OnEditButtonClicked)
					.Content()
					[
							SAssignNew(ButtonLabel, STextBlock)
							.Text(FText::FromString(TEXT("Load episode")))				
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

void UMCGraspingEditorCallback::ShowSaveGraspingStyleWindow()
{
	//Creates the save menu with 2 editable textfields and a button
	TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Save as grasp")))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(NewGraspingStyleNameBox, SEditableTextBox)
			.Text(FText::FromString(TEXT("Enter grasping style name")))
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.OnClicked_Raw(this, &UMCGraspingEditorCallback::OnSaveButtonClicked)
		.Content()
		[
			SAssignNew(ButtonLabel, STextBlock)
			.Text(FText::FromString(TEXT("Save episodes as grasping style")))
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

void UMCGraspingEditorCallback::SaveBoneDatasAsEpisode()
{
	TMap<FString, FMCBoneData> NewEpisodeData = TMap<FString, FMCBoneData>();
	TMap<FString, FRotator> CalculatedBoneRotations;

	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceTransforms;
	int Index = 0;
	//Saves the current bone rotations as bone space. Its needed to load in an episode since only bone space is displayed in a preview.
	for (FTransform BoneTransform : BoneSpaceTransforms) 
	{
		CurrentBoneSpaceTransforms.Add(BoneNames[Index].ToString(), BoneTransform.GetRotation().Rotator());
		Index++;
	}
	//go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FMCBoneData NewBoneData = FMCBoneData();
		//get the rotation of bones
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);
		FRotator *FoundRotation = CurrentBoneSpaceTransforms.Find(NewConstraint->ConstraintBone1.ToString());
		NewBoneData.BoneSpace = *FoundRotation;

		//save the start rotations of all the bones
		if (!StartRotatorsSet)
		{
			StartRotators.Add(NewConstraint->ConstraintBone1.ToString(), QuatBone1);
			StartRotators.Add(NewConstraint->ConstraintBone2.ToString(), QuatBone2);
			NewBoneData.AngularDriveInput = FRotator(0, 0, 0);
			CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), FRotator(0, 0, 0));
		}
		else 
		{
			//calculate how much bone1's roation has changed relative to the start 
			FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(*StartRotators.Find(NewConstraint->ConstraintBone1.ToString())));
			FQuat Quat1Difference = Transform1Difference.GetRotation();
			FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(*StartRotators.Find(NewConstraint->ConstraintBone2.ToString())));
			FQuat Quat2Difference = Transform2Difference.GetRotation();
			//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
			FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
			NewBoneData.AngularDriveInput = Quat.Rotator();
			CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), (Quat.Rotator()));
		}
		NewEpisodeData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}


	//Checks if there are already starting rotations set.
	if (!StartRotatorsSet)
	{
		StartRotatorsSet = true;
	}

	//Checks if there is a need to create a new animation data.
	if (!bFirstCreatedEpisodeData)
	{
		bFirstCreatedEpisodeData = true;
		CreateAnimationData(CalculatedBoneRotations);
	}
	//Show an error message if the new episode could not be added.
	if (!NewGraspAnimationData.AddNewEpisode(NewEpisodeData)) {

		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not add a new episode. Close and open the preview scene again and repeat your last steps."));
	}
	ShowMessageBox(FText::FromString("Saved current position"), FText::FromString("The current hand position was saved. Either add more episodes for the grasp or press Create new grasping style to save all your episodes in a file."));
}

void UMCGraspingEditorCallback::WriteEpisodesToFile()
{
	//You need at least 2 episodes to create a 
	if (NewGraspAnimationData.GetNumberOfEpisodes() < 2) 
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You did not create enough episodes. A grasping style needs at least 2 episodes."));
		return;
	}

	//Save all episodes under the given name and reset all boolean etc.
	UMCWrite Write = UMCWrite();
	NewGraspAnimationData.AnimationName = NewGraspStyle;
	Write.WriteFile(NewGraspAnimationData);
	NewGraspAnimationData = FMCAnimationData();
	bFirstCreatedEpisodeData = false;

	ShowMessageBox(FText::FromString("Saved grap"), FText::FromString("Grasp was saved in an .ini file in the GraspAnimations folder."));
}

void UMCGraspingEditorCallback::EditLoadedGraspingStyle()
{
	if (CurrentGraspEdited.IsEmpty() || CurrentEditedEpisode == 0)
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit grasping style. Either you did not load any grasping style or you are trying to edit the episode 0."));
		return;
	}
	
	FMCAnimationData GraspDataToEdit = UMCRead::ReadFile(DebugMeshComponent->SkeletalMesh->GetFName().ToString(), CurrentGraspEdited);

	TMap<FString, FMCBoneData> NewEpisodeData = TMap<FString, FMCBoneData>();

	TArray<FTransform> BoneSpaceTransforms = DebugMeshComponent->BoneSpaceTransforms;
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceTransforms;
	int Index = 0;
	//Saves the current bone rotations as bone space. Its needed to load in an episode since only bone space is displayed in a preview.
	for (FTransform BoneTransform : BoneSpaceTransforms) 
	{
		CurrentBoneSpaceTransforms.Add(BoneNames[Index].ToString(), BoneTransform.GetRotation().Rotator());
		Index++;
	}
	//go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FMCBoneData NewBoneData = FMCBoneData();
		//get the rotation of bones
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);
		FRotator *FoundRotation = CurrentBoneSpaceTransforms.Find(NewConstraint->ConstraintBone1.ToString());
		NewBoneData.BoneSpace = *FoundRotation;

		//calculate how much bone1's roation has changed relative to the start 
		FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(*StartRotatorsComponentSpace.Find(NewConstraint->ConstraintBone1.ToString())));
		FQuat Quat1Difference = Transform1Difference.GetRotation();
		FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(*StartRotatorsComponentSpace.Find(NewConstraint->ConstraintBone2.ToString())));
		FQuat Quat2Difference = Transform2Difference.GetRotation();
		//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
		FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
		NewBoneData.AngularDriveInput = Quat.Rotator();
		NewEpisodeData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
		
	}

	//Show an error message if the episode could not get overwritten.
	if (!GraspDataToEdit.ReplaceEpisode(CurrentEditedEpisode, NewEpisodeData))
	{
		ShowMessageBox(FText::FromString("Error"), FText::FromString("Could not edit the chosen episode."));
		return;
	}
	UMCWrite::WriteFile(GraspDataToEdit);
	ShowMessageBox(FText::FromString("Edited grasp"), FText::FromString("The grasp was successfuly edited."));
	//Reloads the saved step.
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrentEditedEpisode);
}

TMap<FName, FRotator> UMCGraspingEditorCallback::GetBoneRotations(USkeletalMeshComponent * SkeletalComponent)
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

void UMCGraspingEditorCallback::SetPreviewMeshComponent(UDebugSkelMeshComponent * Component)
{
	DebugMeshComponent = Component;
}

void UMCGraspingEditorCallback::ReadFingerTypes()
{
	TArray<FString> FingerTypes;
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\Plugins\\URealisticGrasping\\Config\\TypeMap.txt";
	const TCHAR* Filepath = *Path;
	FFileHelper::LoadFileToStringArray(FingerTypes, Filepath);
}
void UMCGraspingEditorCallback::CreateAnimationData(TMap<FString, FRotator> EpisodeData)
{
	NewGraspAnimationData.BoneNames = TArray<FString>();
	for (auto& Elem : EpisodeData)
	{
		NewGraspAnimationData.BoneNames.Add(Elem.Key);
	}
	NewGraspAnimationData.SkeletalName = DebugMeshComponent->SkeletalMesh->GetFName().ToString();
}

void UMCGraspingEditorCallback::ChangeBoneRotationsTo(FString GraspingStyle, int EpisodeToEdit)
{
	DebugMeshComponent->SkeletalMesh->Modify();
	DebugMeshComponent->PreviewInstance->ResetModifiedBone();
	FMCAnimationData GraspDataToReadFrom = UMCRead::ReadFile(DebugMeshComponent->SkeletalMesh->GetFName().ToString(), GraspingStyle);

	//Show an error message if one of the parameter are wrong.
	if (GraspDataToReadFrom.AnimationName == "") {
		ShowMessageBox(FText::FromString("Error"), FText::FromString("The chosen grasp does not exist. Make sure the value exists and try again."));
		return;
	}

	if (EpisodeToEdit < 0) {
		ShowMessageBox(FText::FromString("Error"), FText::FromString("You typed in an episode that does not exists. Change the episode to edit and try again."));
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

void UMCGraspingEditorCallback::ShowInstructions(FString Message)
{
	ShowMessageBox(FText::FromString("Help"), FText::FromString(Message));
}

void UMCGraspingEditorCallback::PlayOneEpisode(TMap<FString, FVector> BoneStartLocations, FMCAnimationData PlayData, int Index)
{
	DebugMeshComponent->SkeletalMesh->Modify();

	//Replace the current rotations with the rotations at step 0 for the currently loaded grasping stlye.
	ApplyFingerDataForStartingIndex(PlayData);
	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);

	//Replace the current rotations with the rotations at the given step.
	ApplyFingerDataForIndex(PlayData, Index);

	DebugMeshComponent->PreviewInstance->SetForceRetargetBasePose(true);
}

void UMCGraspingEditorCallback::DiscardAllEpisodes()
{
	NewGraspAnimationData = FMCAnimationData();
	Reset();
	ShowMessageBox(FText::FromString("Discard successful"), FText::FromString("All your recorded Episodes are discarded."));
}

void UMCGraspingEditorCallback::ShowEpisode(bool bForward)
{
	FMCAnimationData HandAnimationData = UMCRead::ReadFile(DebugMeshComponent->SkeletalMesh->GetFName().ToString(), CurrentGraspEdited);
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
		if (CurrentEditedEpisode == MaxEpisode)
		{
			CurrentEditedEpisode = 0;
		}
		else 
		{
			CurrentEditedEpisode += 1;
		}
	}
	else 
	{
		if (CurrentEditedEpisode == 0)
		{
			CurrentEditedEpisode = MaxEpisode;
		}
		else 
		{
			CurrentEditedEpisode -= 1;
		}
	}

	//Show next step.
	PlayOneEpisode(BoneStartLocations, HandAnimationData, CurrentEditedEpisode);
}

void UMCGraspingEditorCallback::Reset()
{
	StartRotatorsSet = false;
	StartRotators.Empty();
	StartLocations.Empty();
	StartRotatorsComponentSpace.Empty();
	StartingBoneRotations.Empty();
	CurrentEditedEpisode = 0;
	CurrentGraspEdited = "";
	NewGraspStyle = "";
	bFirstCreatedEpisodeData = false;
}

void UMCGraspingEditorCallback::SetStartingBoneRotations(TMap<FString, FTransform> BoneRotations)
{
	StartingBoneRotations = BoneRotations;
}

void UMCGraspingEditorCallback::ShowMessageBox(FText Title, FText Message)
{
	FMessageDialog* Instructions = new FMessageDialog();

	//Creates the dialog window.
	Instructions->Debugf(Message,&Title);
}

void UMCGraspingEditorCallback::FillStartingRotatorsInComponentSpace()
{
	//Gets all rotations of the bones in components space.
	for (FConstraintInstance* NewConstraint : DebugMeshComponent->Constraints)
	{
		FQuat QuatBone1 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = DebugMeshComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);

		StartRotatorsComponentSpace.Add(NewConstraint->ConstraintBone1.ToString(), QuatBone1);
		StartRotatorsComponentSpace.Add(NewConstraint->ConstraintBone2.ToString(), QuatBone2);
	}
}

void UMCGraspingEditorCallback::ApplyFingerDataForStartingIndex(FMCAnimationData PlayData)
{
	FMCEpisodeData FingerDataStartingIndex = PlayData.GetPositionDataWithIndex(0);
	TMap<FString, FMCBoneData>* FingerDataMapStartingIndex = FingerDataStartingIndex.GetMap();

	//Apply the rotations at step 0 for the given AnimationData on the DebugMeshComponent
	for (auto BoneDataEntry : *FingerDataMapStartingIndex)
	{
		FRotator BoneData = BoneDataEntry.Value.BoneSpace;
		FRotator SwitchYawPitch = FRotator(BoneData.Pitch, BoneData.Yaw, BoneData.Roll);
		int Index = DebugMeshComponent->GetBoneIndex(FName(*BoneDataEntry.Key));
		FTransform* OldBoneTransform = StartingBoneRotations.Find(BoneDataEntry.Key);
		FVector* OldBoneLocation = StartLocations.Find(BoneDataEntry.Key);
		DebugMeshComponent->SkeletalMesh->RetargetBasePose[Index] = FTransform(BoneData, *OldBoneLocation);
	}
}

void UMCGraspingEditorCallback::ApplyFingerDataForIndex(FMCAnimationData PlayData, int Index)
{
	FMCEpisodeData FingerData = PlayData.GetPositionDataWithIndex(Index);
	TMap<FString, FMCBoneData>* FingerDataMap = FingerData.GetMap();

	//Apply the rotations at a given step for the given AnimationData on the DebugMeshComponent
	for (auto BoneDataEntry : *FingerDataMap)
	{
		FRotator BoneData = BoneDataEntry.Value.BoneSpace;
		FRotator SwitchYawPitch = FRotator(BoneData.Pitch, BoneData.Yaw, BoneData.Roll);
		int BoneIndex = DebugMeshComponent->GetBoneIndex(FName(*BoneDataEntry.Key));
		FTransform OldBoneTransform = DebugMeshComponent->GetBoneTransform(BoneIndex);
		FVector* OldBoneLocation = StartLocations.Find(BoneDataEntry.Key);
		DebugMeshComponent->SkeletalMesh->RetargetBasePose[BoneIndex] = FTransform(BoneData, *OldBoneLocation);
	}
}

void UMCGraspingEditorCallback::FillStartLocations()
{
	//Gets the current starting locations if the DebugMeshComponent
	TArray<FName> BoneNames;
	DebugMeshComponent->GetBoneNames(BoneNames);
	int Index = 0;
	for (FTransform BoneTransform : DebugMeshComponent->BoneSpaceTransforms) 
	{
		StartLocations.Add(BoneNames[Index].ToString(), BoneTransform.GetTranslation());
		Index++;
	}
}

FReply UMCGraspingEditorCallback::OnEditButtonClicked()
{
	CurrentGraspEdited = GraspingStyleBox->GetText().ToString();
	FText EpisodeToEdit = EpisodeBox->GetText();
	CurrentEditedEpisode = FCString::Atoi(*EpisodeToEdit.ToString());

	//Changes bone rotations to the given step for the given grasping stlye
	ChangeBoneRotationsTo(CurrentGraspEdited, CurrentEditedEpisode);
	return FReply::Handled();
}

FReply UMCGraspingEditorCallback::OnSaveButtonClicked()
{
	NewGraspStyle = NewGraspingStyleNameBox->GetText().ToString();

	//Saves a new grasping style under the name given name
	WriteEpisodesToFile();
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE