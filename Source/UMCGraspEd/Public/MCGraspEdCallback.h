// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

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
	* Shows a window where you can set a grasp anim at a specific frame to edit.
	*/
    void ShowFrameEditWindow();

	/*
	* Shows a window where you can set a name for a new grasp anim.
	*/
	void ShowSaveGraspAnimWindow();

	/*
	* Saves the current position of the displayed mesh as a frame.
	*/
	void SaveBoneDatasAsFrame();

	/*
	* Writes all currently recorded frames to a DataAsset.
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
	* Creates a new AnimationData with the given frames.
	* @param FrameData A map containing BoneNames as key and its rotation as value.
	*/
	void CreateAnimationData(TMap<FString, FRotator> FrameData);

	/*
	* Loads a step from a grasp anim.
	* @param GraspAnim the stlye to load.
	* @param StepToEdit the steps that should get displayed in the preview scene.
	*/
	void ChangeBoneRotationsTo(FString GraspAnim, int StepToEdit);

	/*
	* Shows a help windows with the given message.
	* @param Message the message that gets displayed in the window.
	*/
	void ShowInstructions(FString Message);
	
	/*
	* Shows the frame at the given index for the given HandAnimationData in the preview scene.
	* @param BoneStartLocations The starting locations of the hand. Key is the BoneName and value its location.
	* @param PlayData The AnimationData for a grasping stlye.
	* @param Index The index of the frame that should get displayed.
	*/
	void PlayOneFrame(TMap<FString, FVector> BoneStartLocations, FMCAnimationData PlayData, int Index);

	/*
	* Discards all currently recorded frames.
	*/
	void DiscardAllFrames();

	/*
	* Shows a frame.
	* @param bForward true if the next frame should get shown, false for the previous frame.
	*/
	void ShowFrame(bool bForward);

	/*
	* Resets all variables used in this Class.
	*/
	void Reset();

	// Saves all the transforms we need later on, before bones are moved
	void SaveStartTransforms();

private:

	// Current mesh in the editor 
	UDebugSkelMeshComponent* DebugMeshComponent;

	// Our new grasp anim struct 
	FMCAnimationData NewGraspAnimData = FMCAnimationData();

	/**
	* All the bones start transforms in component space.
	* Used to calculate the change in orientation.
	*/
	TMap<FString, FTransform> StartBoneTransCompSpace;

	/**
	* All the bones start locations in bone space.
	*/
	TMap<FString, FVector> StartBoneLocBoneSpace;

	//Creates a map with the names of the bones as FName and their current rotations as FRotator for a given USkeletalMeshComponent.
    TMap<FName, FRotator> GetBoneRotations(USkeletalMeshComponent*  SkeletalComponent);

	void ShowMessageBox(FText Title, FText Message);

	void ApplyBoneDataForIndex(FMCAnimationData PlayData, int Index);

	FReply OnEditButtonClicked();
	FReply OnSaveButtonClicked();

	//The buttons / editable textboxes displayed in the windows.
	TSharedPtr<STextBlock> ButtonLabel;
	TSharedPtr<SEditableTextBox> NewGraspAnimNameBox;
	TSharedPtr<SEditableTextBox> GraspAnimBox;
	TSharedPtr<SEditableTextBox> FrameBox;

	bool bFirstCreatedFrameData = false;
	FString NewGraspAnim = "";
	bool bSavedStartTransforms = false;

	int CurrentEditedFrame = 0;
	FString CurrentGraspEdited = "";

	bool bContinuePlayGrasp = true;
};