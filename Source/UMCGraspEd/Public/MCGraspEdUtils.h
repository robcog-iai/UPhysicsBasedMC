// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "MCGraspAnimDataAsset.h"

// Forward declarations
class UDebugSkelMeshComponent;

/**
 * Callbacks for creating the data assets. Binding happens in UMCGraspEd
 */
class FMCGraspEdUtils
{
public:
	// Default ctor
	FMCGraspEdUtils();

	// Init the helper functions
	void Init(UDebugSkelMeshComponent* DebugMeshComponent);

	// Getter
	bool IsInit() const { return bIsInit; };

	/* Action mappings 'New Grasp' */
	// Saves all the transforms we need later on, before bones are moved
	void SaveMeshInitialState();

	// Saves the current position of the displayed mesh to a new frame
	void AddFrame();

	// Shows a window where you can set a name for the data asset
	void ShowSaveAnimationWindow();

	// Clears all the variables
	void ClearAll();

	// Creates a message box with instructions on how to create a new grasp
	void ShowNewGraspHelpWindow();


	/* Action mappings 'Edit Grasp' */
	// Shows a window where you can select a frame to edit
	void ShowLoadFrameWindow();

	// Updates the loaded frame with the new data
	void UpdateFrame();

	// Loads the next frame
	void ShowNextFrame();

	// Loads the previous frame
	void ShowPreviousFrame();

	// Creates a message box with instructions on how to edit an existing grasp
	void ShowEditGraspHelpWindow();

private:
	// Called from the save animation windows, it saves the animation to the data asset
	FReply OnSaveCallback();

	// Called from the load frame windows, it loads a frame to the debug mesh
	FReply OnLoadCallback();

	// Write data to asset
	void WriteToAsset(const FString& InAssetName);

	// Loads a step from a grasp anim
	void LoadFrame(const FString& GraspAnimName, int32 FrameIndex);

	// Shows the frame at the given index for the given HandAnimationData in the preview scene
	void ShowFrame(TMap<FString, FVector> BoneStartLocations, TArray<FMCGraspAnimFrameData> Frames, int32 Index);

	// Apply the given frame to the debug mesh
	void ApplyFrame(const FMCGraspAnimFrameData& Frame);

	// Read frames from the data asset file
	bool GetFramesFromAssetName(const FString& Name, TArray<FMCGraspAnimFrameData>& OutFrames);

	// Find the grasp animation data asset, return nullptr if not found
	UMCGraspAnimDataAsset* GetDataAsset(const FString& Name);

	// Finds all the GraspDataAssets in a hardcoded folder
	TArray<UMCGraspAnimDataAsset*> GetAllDataAssets();

	// Creates a message dialog box
	void ShowMessageBox(FText Title, FText Message);

private:
	// True when the debug mesh component reference is set
	bool bIsInit;

	// True when the initial state of the debug mesh is stored
	bool bInitialStateCached;

	// Current mesh in the editor 
	UDebugSkelMeshComponent* DebugMeshComponent;

	// Initial locations of the bones in bone space (SkeletalMeshComponent::BoneSpaceTransforms.Location) 
	// (local-space (relative to parent bone))
	TMap<FString, FVector> InitialBoneSpaceLocations;

	// Initial transformation of all the bones in component space (used for calculating the target for the angular motor drives)
	TMap<FString, FTransform> InitialBoneCompSpaceTransf;

	// The grasp animation frames
	TArray<FMCGraspAnimFrameData> AnimFrames;

	// Slate windows buttons/text boxes
	TSharedPtr<class SEditableTextBox> NameTextBox;
	TSharedPtr<class SEditableTextBox> FrameNrTextBox;
	TSharedPtr<class STextBlock> ButtonLabel;
	
	
	FString NewGraspAnimName;
	int32 CurrEditFrameIndex;
	
	FString CurrEditGraspName;

	// TODO The currently loaded data asset for editing
	UMCGraspAnimDataAsset* CurrEditDataAsset;

};