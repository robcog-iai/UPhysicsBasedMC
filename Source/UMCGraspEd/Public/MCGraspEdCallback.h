// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Editor/UnrealEd/Classes/Animation/DebugSkelMeshComponent.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "MCGraspAnimStructs.h"

/**
 * 
 */
class UMCGraspEdCallback
{
public:
	UMCGraspEdCallback();

	/*
	* Shows a window where you can set a grasping style at a specific episode to edit.
	*/
    void ShowFrameEditWindow();

	/*
	* Shows a window where you can set a name for a new grasping stlye.
	*/
	void ShowSaveGraspAnimWindow();

	/*
	* Saves the current position of the displayed mesh as an episode.
	*/
	void SaveBoneDatasAsFrame();

	/*
	* Writes all currently recordes episodes to an .ini file.
	*/
	void WriteFramesToAsset();

	/*
	* Overwrites the currently loaded step with the currently displayed bone rotations.
	*/
	void EditLoadedGraspAnim();

	/*
	* Sets the DebugMeshComponent to work with.
	*/
	void SetPreviewMeshComponent(UDebugSkelMeshComponent* Component);

	/*
	* Creates a new AnimationData with the given episodes.
	* @param EpisodeData A map containing BoneNames as key and its rotation as value.
	*/
	void CreateAnimationData(TMap<FString, FRotator> EpisodeData);

	/*
	* Loads a step from a grasping style.
	* @param GraspingStyle the stlye to load.
	* @param StepToEdit the steps that should get displayed in the preview scene.
	*/
	void ChangeBoneRotationsTo(FString GraspingStyle, int StepToEdit);

	/*
	* Shows a help windows with the given message.
	* @param Message the message that gets displayed in the window.
	*/
	void ShowInstructions(FString Message);
	
	/*
	* Shows the episode at the given index for the given HandAnimationData in the preview scene.
	* @param BoneStartLocations The starting locations of the hand. Key is the BoneName and value its location.
	* @param PlayData The AnimationData for a grasping stlye.
	* @param Index The index of the episode that should get displayed.
	*/
	void PlayOneFrame(TMap<FString, FVector> BoneStartLocations, FMCAnimationData PlayData, int Index);

	/*
	* Discards all currently recorded episodes.
	*/
	void DiscardAllFrames();

	/*
	* Shows an episodes.
	* @param bForward true if the next episode should get shown, false for the previous episode.
	*/
	void ShowFrame(bool bForward);

	/*
	* Resets all variables used in this Class.
	*/
	void Reset();

	/*
	* Sets the starting rotations to the given bone rotations.
	* @param BoneRotations A map containing the BoneName as key and its rotation as value.
	*/
	void SetStartingBoneTransforms(TMap<FString, FTransform> BoneRotations);

	void FillStartingRotatorsInComponentSpace();

private:

	//Creates a map with the names of the bones as FName and their current rotations as FRotator for a given USkeletalMeshComponent.
    TMap<FName, FRotator> GetBoneRotations(USkeletalMeshComponent*  SkeletalComponent);

	void ShowMessageBox(FText Title, FText Message);

	void FillStartLocations();

	//Applies FingerData for different indices.
	void ApplyFingerDataForStartingIndex(FMCAnimationData PlayData);
	void ApplyFingerDataForIndex(FMCAnimationData PlayData, int Index);


	FReply OnEditButtonClicked();
	FReply OnSaveButtonClicked();

	/**
	* All the bones start locations in bone space.
	* Used to load frames back into editor.
	*/
	TMap<FString, FVector> StartBoneLocations;

	/**
	* All the bones start transforms in bone space.
	* Used to load frames back into editor.
	*/
	TMap<FString, FTransform> StartBoneTransforms;

	/**
	* All the bones start rotations in component space.
	* Used to calculate the change in orientation. 
	*/
	TMap<FString, FQuat> StartRotatorsComponentSpace;

	//The buttons / editable textboxes displayed in the windows.
	TSharedPtr<STextBlock> ButtonLabel;
	TSharedPtr<SEditableTextBox> NewGraspAnimNameBox;
	TSharedPtr<SEditableTextBox> GraspAnimBox;
	TSharedPtr<SEditableTextBox> FrameBox;

	bool bFirstCreatedFrameData = false;
	FString NewGraspAnim = "";
	bool StartRotatorsSet = false;

	int CurrentEditedFrame = 0;
	FString CurrentGraspEdited = "";

	UDebugSkelMeshComponent* DebugMeshComponent;

	FMCAnimationData NewGraspAnimData = FMCAnimationData();

	bool bContinuePlayGrasp = true;
};