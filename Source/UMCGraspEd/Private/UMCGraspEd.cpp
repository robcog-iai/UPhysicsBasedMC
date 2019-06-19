// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UMCGraspEd.h"
#include "MCGraspEdUtils.h"
#include "PersonaModule.h"
#include "IPersonaToolkit.h"
#include "ISkeletalMeshEditorModule.h"
#include "MCGraspEdStyle.h"
#include "MCGraspEdCommands.h"

#include "Widgets/SWidget.h"

#define LOCTEXT_NAMESPACE "FUMCGraspEd"

// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
void FUMCGraspEd::StartupModule()
{
	UMCGraspEdStyle::Initialize();
	UMCGraspEdStyle::ReloadTextures();

	// Creates the toolbar extension for the grasp animation creation and editing
	CreateToolBarExtensions();

	// Loads the Persona module and binds an action to its OnPreviewSceneCreatedDelegate
	FPersonaModule& PersonaModule =
		FModuleManager::Get().LoadModuleChecked<FPersonaModule>("Persona");	
	PersonaModule.OnPreviewSceneCreated().AddLambda(
		[this](const TSharedRef<IPersonaPreviewScene>& PreviewScene) { CallbackCreateGraspHelper(PreviewScene); });
}

// Called during shutdown to clean up the module
void FUMCGraspEd::ShutdownModule()
{
	// For modules that support dynamic reloading, we call this function before unloading the module.
	UMCGraspEdCommands::Unregister();
}

// Called when a new preview scene is created, it sets the debug mesh and creates the grasp helper
void FUMCGraspEd::CallbackCreateGraspHelper(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	// Initialize the helper class with the debug mesh	
	TSharedRef<IPersonaToolkit> PersonaToolKitRef = InPreviewScene.Get().GetPersonaToolkit();
	UDebugSkelMeshComponent* DebugMeshComponent = PersonaToolKitRef.Get().GetPreviewMeshComponent();	

	// Create / reset the helper class
	if (!GraspEdUtils.IsValid())
	{
		GraspEdUtils = MakeShareable<FMCGraspEdUtils>(new FMCGraspEdUtils());
	}
	else
	{
		GraspEdUtils.Reset();
		GraspEdUtils = MakeShareable<FMCGraspEdUtils>(new FMCGraspEdUtils());
	}

	GraspEdUtils->Init(DebugMeshComponent);

	if (GraspEdUtils->IsInit())
	{
		// Map the dropdown entries commands to their callbacks
		MapCommands();
	}
}

// Add the 'New Grasp' and 'Edit Grasp' toolbar extensions
void FUMCGraspEd::CreateToolBarExtensions()
{
	// Load the ISkeletalMeshEditorModule and add a new button to its menu bar.
	ISkeletalMeshEditorModule& SkeletalMeshEditorModule =
		FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>("SkeletonEditor");

	// Create a toolbar extender
	TSharedPtr<FExtender> ToolBarExtender = MakeShareable(new FExtender);
	
	// Create the extensions
	ToolBarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		CommandsList,
		FToolBarExtensionDelegate::CreateRaw(this, &FUMCGraspEd::CallbackCreateNewGraspToolBar)
	);
	ToolBarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		CommandsList,
		FToolBarExtensionDelegate::CreateRaw(this, &FUMCGraspEd::CallbackCreateEditGraspToolBar)
	);
	
	// Add the extensions to the toolbar
	SkeletalMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarExtender);
}

// Creates toolbar extension 'New Grasp'
void FUMCGraspEd::CallbackCreateNewGraspToolBar(FToolBarBuilder & Builder)
{
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FUMCGraspEd::CallbackCreateNewGraspEntries),
		LOCTEXT("NewGrasp", "New Grasp"),
		LOCTEXT("NewGrasp_Tooltip", "Buttons for creating a new grasp animation"),
		FSlateIcon(UMCGraspEdStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}

// Creates toolbar extension 'Edit Grasp'
void FUMCGraspEd::CallbackCreateEditGraspToolBar(FToolBarBuilder & Builder)
{
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FUMCGraspEd::CallbackCreateEditGraspEntries),
		LOCTEXT("Edit", "Edit Grasp Anim"),
		LOCTEXT("Edit_Tooltip", "Options to edit an existing grasping animation"),
		FSlateIcon(UMCGraspEdStyle::GetStyleSetName(), "GraspingEditor.DebugOptionToolBar"),
		false
	);
}

// Creates toolbar extension 'New Grasp' entries
TSharedRef<SWidget> FUMCGraspEd::CallbackCreateNewGraspEntries()
{
	// Creates all of the drop down entries of the "New Grasp" button.
	FMenuBuilder MenuBuilder(false, CommandsList.ToSharedRef());
	
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();
	MenuBuilder.BeginSection("New Grasp");
	{
		MenuBuilder.AddMenuEntry(Commands.SaveMeshInitialStateCmd);
		MenuBuilder.AddMenuEntry(Commands.AddFrameCmd);
		MenuBuilder.AddMenuEntry(Commands.SaveAnimCmd);
		MenuBuilder.AddMenuEntry(Commands.ClearAllCmd);
		MenuBuilder.AddMenuEntry(Commands.HelpNewGraspCmd);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

// Creates toolbar extension 'Edit Grasp' entries
TSharedRef<SWidget> FUMCGraspEd::CallbackCreateEditGraspEntries()
{
	// Creates all of the drop down entries of the "Edit Grasp Animation" button.
	FMenuBuilder MenuBuilder(false, CommandsList.ToSharedRef());
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();
	MenuBuilder.BeginSection("Edit Grasp");
	{
		MenuBuilder.AddMenuEntry(Commands.SaveMeshInitialStateCmd);
		MenuBuilder.AddMenuEntry(Commands.LoadFrameCmd);
		MenuBuilder.AddMenuEntry(Commands.UpdateFrameCmd);
		MenuBuilder.AddMenuEntry(Commands.NextFrameCmd);
		MenuBuilder.AddMenuEntry(Commands.PrevFrameCmd);
		MenuBuilder.AddMenuEntry(Commands.HelpEditGraspCmd);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

// Map the dropdown entries for 'New Grasp'
void FUMCGraspEd::MapCommands()
{
	// Use this method to register commands. Usually done in StartupModule()
	UMCGraspEdCommands::Register();

	CommandsList = MakeShareable(new FUICommandList);
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();

	if (!GraspEdUtils.IsValid())
	{
		return;
	}

	TSharedRef<FMCGraspEdUtils> GraspEdHelperRef = GraspEdUtils.ToSharedRef();
	
	// 'New Grasp' actions mapping
	CommandsList->MapAction(
		Commands.SaveMeshInitialStateCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::SaveMeshInitialState),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.AddFrameCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::AddFrame),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.SaveAnimCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ShowSaveAnimationWindow),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.ClearAllCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ClearAll),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.HelpNewGraspCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ShowNewGraspHelpWindow),
		FCanExecuteAction()
	);

	// 'Edit Grasp' actions mapping
	CommandsList->MapAction(
		Commands.LoadFrameCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ShowLoadFrameWindow),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.UpdateFrameCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::UpdateFrame),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.NextFrameCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ShowNextFrame),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.PrevFrameCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ShowPreviousFrame),
		FCanExecuteAction()
	);
	CommandsList->MapAction(
		Commands.HelpEditGraspCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdUtils::ShowEditGraspHelpWindow),
		FCanExecuteAction()
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUMCGraspEd, UMCGraspEd)