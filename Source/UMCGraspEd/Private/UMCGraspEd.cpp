// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UMCGraspEd.h"
#include "MCGraspEdStyle.h"
#include "MCGraspEdCommands.h"
#include "MCGraspEdCallback.h"
#include "PersonaModule.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"
#include "Engine.h"
#include "LevelEditor.h"
#include "ISkeletalMeshEditor.h"
#include "ISkeletalMeshEditorModule.h"
#include "ISkeletonEditor.h"
#include "Editor/UnrealEd/Classes/Animation/DebugSkelMeshComponent.h"
#include "ISkeletonEditorModule.h"
#include "Editor/AnimGraph/Classes/AnimPreviewInstance.h"

#define LOCTEXT_NAMESPACE "FUMCGraspEd"

void FUMCGraspEd::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UMCGraspEdStyle::Initialize();
	UMCGraspEdStyle::ReloadTextures();

	//Initializes the drop down menu.
	InitializeUIButtons();

	//Creates the button in the menu bar fo the skeleton editor.
	CreateButton();

	//Loads the Persona module and binds an action to its OnPreviewSceneCreatedDelegate
	FPersonaModule& PersonaModule =
		FModuleManager::Get().LoadModuleChecked<FPersonaModule>("Persona");

	OnPreviewSceneCreatedDelegate = PersonaModule.OnPreviewSceneCreated().AddLambda([this](const TSharedRef<IPersonaPreviewScene>& param) { this->OnPreviewCreation(param); });
}

void FUMCGraspEd::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UMCGraspEdCommands::Unregister();
}

void FUMCGraspEd::InitializeUIButtons()
{
	UMCGraspEdCommands::Register();
	EditorCallback = UMCGraspEdCallback();
	PluginCommandListCreateSection = MakeShareable(new FUICommandList);
	PluginCommandListEditSection = MakeShareable(new FUICommandList);
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();

	//Commands for the "New Grasp Animation " button
	PluginCommandListCreateSection->MapAction(
		Commands.Initialize,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::InitializeStartTransforms),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.CreateGraspAnim,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowSaveGraspAnimWindow),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.SaveGraspPosition,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::SaveBoneDatasAsFrame),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.DiscardNewGraspAnim,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::DiscardAllFrames),
		FCanExecuteAction()
	);

	PluginCommandListCreateSection->MapAction(
		Commands.ShowCreateHelp,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowCreateHelp),
		FCanExecuteAction()
	);

	//Commands for the "Edit Grasp Animation" button
	PluginCommandListEditSection->MapAction(
		Commands.Initialize,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::InitializeStartTransforms),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.LoadGraspAnim,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowFrameEditWindow),
		FCanExecuteAction()
	); 

	PluginCommandListEditSection->MapAction(
		Commands.EditGraspPosition,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::EditLoadedGraspAnim),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.ShowNextFrame,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowNextFrame),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.ShowPreviousFrame,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowPreviousFrame),
		FCanExecuteAction()
	);

	PluginCommandListEditSection->MapAction(
		Commands.ShowEditHelp,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowEditHelp),
		FCanExecuteAction()
	);
}

void FUMCGraspEd::CreateButton()
{
	//Load the ISkeletalMeshEditorModule and add a new button to its menu bar.
	ISkeletalMeshEditorModule& SkeletalMeshEditorModule =
		FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>("SkeletonEditor");
	// Add toolbar entry
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	//Creates the "New Grasp Animation" button
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		PluginCommandListCreateSection,
		FToolBarExtensionDelegate::CreateRaw(this, &FUMCGraspEd::AddCreateOptions)
	);
	//Creates the "Edit Grasp Animation" button
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		PluginCommandListEditSection,
		FToolBarExtensionDelegate::CreateRaw(this, &FUMCGraspEd::AddEditOptions)
	);
	
	SkeletalMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}


TSharedRef<SWidget> FUMCGraspEd::CreateOptionMenu()
{
	//Creates all of the drop down entries of the "New Grasp Animation" button.
	FMenuBuilder CreateBuilder(false, PluginCommandListCreateSection.ToSharedRef());
	
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();
	CreateBuilder.BeginSection("New Grasp Animation");
	{
		CreateBuilder.AddMenuEntry(Commands.Initialize);
		CreateBuilder.AddMenuEntry(Commands.SaveGraspPosition);
		CreateBuilder.AddMenuEntry(Commands.CreateGraspAnim);
		CreateBuilder.AddMenuEntry(Commands.DiscardNewGraspAnim);
		CreateBuilder.AddMenuEntry(Commands.ShowCreateHelp);
	}
	CreateBuilder.EndSection();

	return CreateBuilder.MakeWidget();
}

TSharedRef<SWidget> FUMCGraspEd::EditOptionMenu()
{
	//Creates all of the drop down entries of the "Edit Grasp Animation" button.
	FMenuBuilder EditBuilder(false, PluginCommandListEditSection.ToSharedRef());
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();
	EditBuilder.BeginSection("Edit Grasp Animation");
	{
		EditBuilder.AddMenuEntry(Commands.Initialize);
		EditBuilder.AddMenuEntry(Commands.LoadGraspAnim);
		EditBuilder.AddMenuEntry(Commands.EditGraspPosition);
		EditBuilder.AddMenuEntry(Commands.ShowNextFrame);
		EditBuilder.AddMenuEntry(Commands.ShowPreviousFrame);
		EditBuilder.AddMenuEntry(Commands.ShowEditHelp);
	}
	EditBuilder.EndSection();

	return EditBuilder.MakeWidget();
}

void FUMCGraspEd::AddCreateOptions(FToolBarBuilder & Builder)
{
	UMCGraspEdCommands GraspingEditorCommands;

	//Adds the drop down menu to the button.
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FUMCGraspEd::CreateOptionMenu),
		LOCTEXT("Create", "New Grasp Anim"),
		LOCTEXT("Create_Tooltip", "Options to create a grasping animation"),
		FSlateIcon(UMCGraspEdStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}

void FUMCGraspEd::AddEditOptions(FToolBarBuilder & Builder)
{
	UMCGraspEdCommands GraspingEditorCommands;

	//Adds the drop down menu to the button.
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FUMCGraspEd::EditOptionMenu),
		LOCTEXT("Edit", "Edit Grasp Anim"),
		LOCTEXT("Edit_Tooltip", "Options to edit an existing grasping animation"),
		FSlateIcon(UMCGraspEdStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}


void FUMCGraspEd::OnPreviewCreation(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	//When a preview scene is created get its DebugMeshComponent and also reset the EditorCallBack
	TSharedRef<IPersonaToolkit> PersonaToolKitRef = InPreviewScene.Get().GetPersonaToolkit();
	DebugMeshComponent = PersonaToolKitRef.Get().GetPreviewMeshComponent();
	EditorCallback.Reset();
}

void FUMCGraspEd::InitializeStartTransforms()
{
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.SaveStartTransforms();
	StartRotationsInitialized = true;
}

void FUMCGraspEd::ShowFrameEditWindow()
{
	if (!StartRotationsInitialized)
	{
		FString Message = "Please press start before editing any grasps";
		EditorCallback.ShowInstructions(Message);
		return;
	}
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowFrameEditWindow();
}

void FUMCGraspEd::WriteFramesToAsset()
{
	//Writes all currently recorded frames to a DataAsset
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.WriteFramesToAsset();
}

void FUMCGraspEd::ShowSaveGraspAnimWindow()
{
	//Shows a window where you can enter a new for a newly created grasping stlye
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowSaveGraspAnimWindow();

}

void FUMCGraspEd::SaveBoneDatasAsFrame()
{
	if(!StartRotationsInitialized)
	{
		FString Message = "Please press start before creating any frames";
		EditorCallback.ShowInstructions(Message);
		return;
	}
	//Saves the current mesh position as a frame
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.SaveBoneDatasAsFrame();
}

void FUMCGraspEd::EditLoadedGraspAnim()
{
	//Overwrites the loaded step for a grasping stlye with the currently displayed hand position
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.EditLoadedGraspAnim();
}

void FUMCGraspEd::ShowCreateHelp()
{
	//Shows help for the "Create grasp" button
	FString Message = "Start:\nStarts the process of creating a new grasping animation. Always press this first.\n\nAdd Frame:\nSave the current bone positions as a frame/step for the animation. You need atleast 2 frames for an animation.\n\nSave:\nSaves all frames as a DataAsset, that can then be added to a GraspController.\n\nClear All:\nDeletes all frames.\n\n";
	EditorCallback.ShowInstructions(Message);
}

void FUMCGraspEd::ShowEditHelp()
{
	//Shows help for the "Edit grasp" button
	FString Message = "Load Grasp Animation:\nLoads the specified animation at the specified frame.\n\nUpdate Frame:\nUpdates the current frame with the current bone rotations. You can't edit frame 0.\n\nGoto Next Frame:\nGoes to the next frame.\n\nGoto Privious Frame:\nGoes to privious frame.\n\n";
	EditorCallback.ShowInstructions(Message);
}

void FUMCGraspEd::DiscardAllFrames()
{
	//Discards all currently recorded frames
	EditorCallback.DiscardAllFrames();
}

void FUMCGraspEd::ShowNextFrame()
{
	//Shows the next frame depending on the one currently displayed
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowFrame(true);
}

void FUMCGraspEd::ShowPreviousFrame()
{
	//Shows the previous frame depending on the one currently displayed
	EditorCallback.SetPreviewMeshComponent(DebugMeshComponent);
	EditorCallback.ShowFrame(false);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUMCGraspEd, URealisticGrasping)