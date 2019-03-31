// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Runtime/SlateCore/Public/Widgets/SWidget.h"
#include "IPersonaPreviewScene.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/UnrealEd/Classes/Animation/DebugSkelMeshComponent.h"
#include "MCGraspEdCallback.h"
#include "PersonaModule.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"
#include <functional>

class FUMCGraspEd : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:

	void InitializeUIButtons();
	void CreateButton();

	//Create the "Create grasp" button
	TSharedRef<SWidget> CreateOptionMenu();
	void AddCreateOptions(FToolBarBuilder& Builder);
	TSharedPtr<class FUICommandList> PluginCommandListCreateSection;

	//Create the "Edit grasp" button
	TSharedRef<SWidget> EditOptionMenu();
	void AddEditOptions(FToolBarBuilder& Builder);
	TSharedPtr<class FUICommandList> PluginCommandListEditSection;

	//Gets called when a new preview scene is created
    void OnPreviewCreation(const TSharedRef<IPersonaPreviewScene>& InPreviewScene);

	//Prepares and calls actions on the EditorCallback
	void InitializeStartTransforms();
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

	UMCGraspEdCallback EditorCallback;

	FDelegateHandle OnPreviewSceneCreatedDelegate;

	//The MeshComponent currently displayed in a preview scene.
	UDebugSkelMeshComponent* DebugMeshComponent;

	bool StartRotationsInitialized = false;
};