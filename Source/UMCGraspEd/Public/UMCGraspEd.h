// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
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
	TSharedRef<class SWidget> CallbackCreateNewGraspEntries();
	TSharedRef<class SWidget> CallbackCreateEditGraspEntries();

	// Map the dropdown entries actions
	void MapCommands();

private:
	// Command lists
	TSharedPtr<class FUICommandList> CommandsList;

	// Holds the functions to which the actions are bound
	TSharedPtr<class FMCGraspEdUtils> GraspEdUtils;
};