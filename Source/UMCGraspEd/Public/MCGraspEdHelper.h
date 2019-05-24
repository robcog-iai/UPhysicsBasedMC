// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"

/**
 * Callbacks for creating the data assets. Binding happens in UMCGraspEd
 */
class FMCGraspEdHelper
{
public:
	// Default ctor
	FMCGraspEdHelper();

	// Init the helper functions
	void Init(class UDebugSkelMeshComponent* DebugMeshComponent);



	// Saves the current position of the displayed mesh to a new frame
	void SaveToFrame();

	// Writes all currently recorded frames to the data asset.
	void WriteFramesToAsset();

	// Overwrites the currently loaded step with the currently displayed bone rotations.
	void EditLoadedGraspAnim();

	// Loads a step from a grasp anim
	void ChangeBoneRotationsTo(const FString& GraspAnimName, int32 FrameIndex);
	
	// Shows a help windows with the given message.
	void ShowInstructions(FString Message);

	// Shows the frame at the given index for the given HandAnimationData in the preview scene
	void PlayOneFrame(TMap<FString, FVector> BoneStartLocations, TArray<FMCGraspAnimFrameData> Frames, int32 Index);

	// Discards all currently recorded frames.
	void DiscardAllFrames();

	// TODO split into two (ShowNextFrame ShowPrevFrame)
	// Shows a frame. bForward true if the next frame should get shown, false for the previous frame.
	void ShowFrame(bool bForward);

	// Resets all variables used in this Class.
	void Reset();



	// true if the helper is init
	bool IsInit() const { return bIsInit; };


	// Shows a window where you can select a frame to edit
	void ShowFrameEditWindow();

	// Shows a window where you can set a name for the data asset
	void ShowSaveGraspAnimWindow();

	void ShowCreateHelp();

	// Saves all the transforms we need later on, before bones are moved
	void SaveStartTransforms();

private:
	// Creates a message dialog box
	void ShowMessageBox(FText Title, FText Message);

	// Apply the given frame to the debug mesh
	void ApplyFrame(const FMCGraspAnimFrameData& Frame);

	// Read frames from the data asset file
	bool ReadFramesFromName(const FString& Name, TArray<FMCGraspAnimFrameData>& OutFrames);

	// Find the grasp animation data asset, return nullptr if not found
	UMCGraspAnimDataAsset* GetAnimGraspDataAsset(const FString& Name);

	// Returns all GraspDataAssets that can be read
	TArray<UMCGraspAnimDataAsset*> LoadAllAssets();

	// Finds all the GraspDataAssets in a hardcoded folder
	TArray<UMCGraspAnimDataAsset*> OnRegistryLoaded();

	// Create and save data asset
	void WriteToDataAsset(const FString& InAnimName, const TArray<FMCGraspAnimFrameData>& InFrames);

	// Buttons callbacks
	FReply OnEditButtonClicked();
	FReply OnSaveButtonClicked();

private:
	// true if the helper has been initialized
	bool bIsInit;

	// Current mesh in the editor 
	UDebugSkelMeshComponent* DebugMeshComponent;

	// Our new grasp anim struct 
	TArray<FMCGraspAnimFrameData> NewFrames;

	// All the bones start transforms in component space. Used to calculate the change in orientation.	
	TMap<FString, FTransform> StartBoneTransCompSpace;

	// All the bones start locations in bone space.
	TMap<FString, FVector> StartBoneLocBoneSpace;

	// Creates a map with the names of the bones as FName and their current rotations as FRotator for a given USkeletalMeshComponent.
	TMap<FName, FRotator> GetBoneRotations(USkeletalMeshComponent*  SkeletalComponent);

	// Buttons / editable text boxes displayed in the windows.
	TSharedPtr<STextBlock> ButtonLabel;
	TSharedPtr<SEditableTextBox> NewGraspAnimNameBox;
	TSharedPtr<SEditableTextBox> GraspAnimBox;
	TSharedPtr<SEditableTextBox> FrameBox;

	bool bFirstCreatedFrameData;
	bool bSavedStartTransforms;
	bool bContinuePlayGrasp;

	FString NewGraspAnimName;
	int32 CurrEditFrameIndex;
	FString CurrGraspName;

};