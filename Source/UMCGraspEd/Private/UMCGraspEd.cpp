// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UMCGraspEd.h"
#include "PersonaModule.h"
#include "IPersonaToolkit.h"
#include "ISkeletalMeshEditorModule.h"
#include "MCGraspEdHelper.h"
#include "MCGraspEdStyle.h"
#include "MCGraspEdCommands.h"

#define LOCTEXT_NAMESPACE "FUMCGraspEd"

void FUMCGraspEd::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
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

// Called when a new preview scene is created, it sets the debug mesh and creates the grasp helper
void FUMCGraspEd::CallbackCreateGraspHelper(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	// Initialize the helper class with the debug mesh	
	TSharedRef<IPersonaToolkit> PersonaToolKitRef = InPreviewScene.Get().GetPersonaToolkit();
	UDebugSkelMeshComponent* DebugMeshComponent = PersonaToolKitRef.Get().GetPreviewMeshComponent();	

	// Create / reset the helper class
	if (!GraspEdHelper.IsValid())
	{
		GraspEdHelper = MakeShareable<FMCGraspEdHelper>(new FMCGraspEdHelper());
	}
	else
	{
		GraspEdHelper.Reset();
		GraspEdHelper = MakeShareable<FMCGraspEdHelper>(new FMCGraspEdHelper());
	}

	GraspEdHelper->Init(DebugMeshComponent);

	if (GraspEdHelper->IsInit())
	{
		// Map the dropdown entries commands to their callbacks
		MapCommandsNewGrasp();
		MapCommandsEditGrasp();
	}
}

void FUMCGraspEd::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UMCGraspEdCommands::Unregister();
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
		NewGraspCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FUMCGraspEd::CallbackCreateNewGraspToolBar)
	);
	ToolBarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		EditGraspCommandList,
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
	FMenuBuilder CreateBuilder(false, NewGraspCommandList.ToSharedRef());
	
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();
	CreateBuilder.BeginSection("New Grasp");
	{
		CreateBuilder.AddMenuEntry(Commands.InitGraspCmd);
		CreateBuilder.AddMenuEntry(Commands.SaveFrameCmd);
		CreateBuilder.AddMenuEntry(Commands.SaveAnimCmd);
		CreateBuilder.AddMenuEntry(Commands.ClearAnimCmd);
		CreateBuilder.AddMenuEntry(Commands.ShowHelpNewGraspCmd);
	}
	CreateBuilder.EndSection();

	return CreateBuilder.MakeWidget();
}

// Creates toolbar extension 'Edit Grasp' entries
TSharedRef<SWidget> FUMCGraspEd::CallbackCreateEditGraspEntries()
{
	//Creates all of the drop down entries of the "Edit Grasp Animation" button.
	FMenuBuilder EditBuilder(false, EditGraspCommandList.ToSharedRef());
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();
	EditBuilder.BeginSection("Edit Grasp");
	{
		EditBuilder.AddMenuEntry(Commands.InitGraspCmd);
		EditBuilder.AddMenuEntry(Commands.LoadGraspAnimFrameCommand);
		EditBuilder.AddMenuEntry(Commands.UpdateFraspAnimFrame);
		EditBuilder.AddMenuEntry(Commands.GotoNextGraspAnimFrameCommand);
		EditBuilder.AddMenuEntry(Commands.GotoPrevGraspAnimFrameCommand);
		EditBuilder.AddMenuEntry(Commands.ShowEditGraspAnimHelpCommand);
	}
	EditBuilder.EndSection();

	return EditBuilder.MakeWidget();
}

// Map the dropdown entries for 'New Grasp'
void FUMCGraspEd::MapCommandsNewGrasp()
{
	// Use this method to register commands. Usually done in StartupModule()
	UMCGraspEdCommands::Register();

	NewGraspCommandList = MakeShareable(new FUICommandList);
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();

	if (!GraspEdHelper.IsValid())
	{
		return;
	}

	TSharedRef<FMCGraspEdHelper> GraspEdHelperRef = GraspEdHelper.ToSharedRef();
	
	NewGraspCommandList->MapAction(
		Commands.InitGraspCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdHelper::SaveStartTransforms),
		FCanExecuteAction()
	);
	NewGraspCommandList->MapAction(
		Commands.SaveFrameCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdHelper::SaveToFrame),
		FCanExecuteAction()
	);
	NewGraspCommandList->MapAction(
		Commands.SaveAnimCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdHelper::ShowSaveGraspAnimWindow),
		FCanExecuteAction()
	);
	NewGraspCommandList->MapAction(
		Commands.ClearAnimCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdHelper::DiscardAllFrames),
		FCanExecuteAction()
	);
	NewGraspCommandList->MapAction(
		Commands.ShowHelpNewGraspCmd,
		FExecuteAction::CreateSP(GraspEdHelperRef, &FMCGraspEdHelper::ShowCreateHelp),
		FCanExecuteAction()
	);

}

// Map the dropdown entries for 'Edit Grasp'
void FUMCGraspEd::MapCommandsEditGrasp()
{
	EditGraspCommandList = MakeShareable(new FUICommandList);
	const UMCGraspEdCommands& Commands = UMCGraspEdCommands::Get();

	EditGraspCommandList->MapAction(
		Commands.LoadGraspAnimFrameCommand,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowFrameEditWindow),
		FCanExecuteAction()
	);
	EditGraspCommandList->MapAction(
		Commands.UpdateFraspAnimFrame,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::EditLoadedGraspAnim),
		FCanExecuteAction()
	);
	EditGraspCommandList->MapAction(
		Commands.GotoNextGraspAnimFrameCommand,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowNextFrame),
		FCanExecuteAction()
	);
	EditGraspCommandList->MapAction(
		Commands.GotoPrevGraspAnimFrameCommand,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowPreviousFrame),
		FCanExecuteAction()
	);
	EditGraspCommandList->MapAction(
		Commands.ShowEditGraspAnimHelpCommand,
		FExecuteAction::CreateRaw(this, &FUMCGraspEd::ShowEditHelp),
		FCanExecuteAction()
	);
}


void FUMCGraspEd::ShowFrameEditWindow()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->ShowFrameEditWindow();
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

void FUMCGraspEd::WriteFramesToAsset()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->WriteFramesToAsset();
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}	
}

void FUMCGraspEd::ShowSaveGraspAnimWindow()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->ShowSaveGraspAnimWindow();
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

void FUMCGraspEd::SaveBoneDatasAsFrame()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->SaveToFrame();
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

void FUMCGraspEd::EditLoadedGraspAnim()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->EditLoadedGraspAnim();
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

void FUMCGraspEd::ShowCreateHelp()
{
	if (GraspEdHelper.IsValid())
	{
		FString Message = "Start:\nStarts the process of creating a new grasping animation. Always press this first.\n\nAdd Frame:\nSave the current bone positions as a frame/step for the animation. You need atleast 2 frames for an animation.\n\nSave:\nSaves all frames as a DataAsset, that can then be added to a GraspController.\n\nClear All:\nDeletes all frames.\n\n";
		GraspEdHelper->ShowInstructions(Message);
	}
}

void FUMCGraspEd::ShowEditHelp()
{
	if (GraspEdHelper.IsValid())
	{
		FString Message = "Load Grasp Animation:\nLoads the specified animation at the specified frame.\n\nUpdate Frame:\nUpdates the current frame with the current bone rotations. You can't edit frame 0.\n\nGoto Next Frame:\nGoes to the next frame.\n\nGoto Privious Frame:\nGoes to privious frame.\n\n";
		GraspEdHelper->ShowInstructions(Message);
	}
}

void FUMCGraspEd::DiscardAllFrames()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->DiscardAllFrames();
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

void FUMCGraspEd::ShowNextFrame()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->ShowFrame(true);
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

void FUMCGraspEd::ShowPreviousFrame()
{
	if (GraspEdHelper.IsValid())
	{
		if (GraspEdHelper->IsInit())
		{
			GraspEdHelper->ShowFrame(false);
		}
		else
		{
			FString Message = "Please press init before anything";
			GraspEdHelper->ShowInstructions(Message);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUMCGraspEd, UMCGraspEd)