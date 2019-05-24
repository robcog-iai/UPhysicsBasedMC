// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCommands.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

// Describe and instantiate the commands 
void UMCGraspEdCommands::RegisterCommands() 
{
	UI_COMMAND(InitGraspCmd, "Init",
		"Starts the process of creating or editing a grasp animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SaveFrameCmd, "Add Frame",
		"Add current position to animation as a frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SaveAnimCmd, "Save",
		"Save the grasp animation to a data asset", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ClearAnimCmd, "Clear All",
		"Clears all animation frames", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(LoadGraspAnimFrameCommand, "Load Frame",
		"Loads a specific grasp animation frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(UpdateFraspAnimFrame, "Update Frame",
		"Updates the loaded frame with the new data", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(GotoNextGraspAnimFrameCommand, "Goto Next Frame",
		"Load next frame from the animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(GotoPrevGraspAnimFrameCommand, "Goto Previous Frame",
		"Load previous frame from the animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowHelpNewGraspCmd, "Help",
		"Help for creating a new grasp animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowEditGraspAnimHelpCommand, "Help",
		"Help for editing an existing grasp animation", EUserInterfaceActionType::Button, FInputChord());
}
#undef LOCTEXT_NAMESPACE