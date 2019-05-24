// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Widgets/SWidget.h"
#include "IPersonaPreviewScene.h"

class FUMCGraspEd : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	// Called when a new preview scene is created, it sets the debug mesh and creates the grasp helper
	void CallbackCreateGraspHelper(const TSharedRef<IPersonaPreviewScene>& InPreviewScene);

	// Add the 'New Grasp' and 'Edit Grasp' toolbar extensions with the dropdown entries
	void CreateToolBarExtensions();
	
	// Callbacks for extending the toolbar with the combo buttons and its entries
	void CallbackCreateNewGraspToolBar(FToolBarBuilder& Builder);
	void CallbackCreateEditGraspToolBar(FToolBarBuilder& Builder);
	TSharedRef<SWidget> CallbackCreateNewGraspEntries();
	TSharedRef<SWidget> CallbackCreateEditGraspEntries();

	// Map dropdown entries
	void MapCommandsNewGrasp();
	void MapCommandsEditGrasp();

	// Prepares and calls actions on the EditorCallback
	//void Init();
	void ShowFrameEditWindow();
	void WriteFramesToAsset();
	void ShowSaveGraspAnimWindow();
	void SaveBoneDatasAsFrame();
	void EditLoadedGraspAnim();
	void ShowCreateHelp();
	void ShowEditHelp();
	void DiscardAllFrames();
	void ShowNextFrame();
	void ShowPreviousFrame();

private:
	// Command lists
	TSharedPtr<class FUICommandList> NewGraspCommandList;
	TSharedPtr<class FUICommandList> EditGraspCommandList;

	// Grasp helper pointer
	TSharedPtr<class FMCGraspEdHelper> GraspEdHelper;
};