// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCommands.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

// Describe and instantiate the commands 
void UMCGraspEdCommands::RegisterCommands() 
{
	UI_COMMAND(SaveMeshInitialStateCmd, "Save Mesh Initial State",
		"Store the initial state of the debug mesh", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddFrameCmd, "Add Frame",
		"Add current state to animation as a frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SaveAnimCmd, "Save",
		"Save the grasp animation to a data asset", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ClearAllCmd, "Clear All",
		"Clears all animation frames", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(HelpNewGraspCmd, "Help",
		"Help for creating a new grasp animation", EUserInterfaceActionType::Button, FInputChord());



	UI_COMMAND(LoadFrameCmd, "Load Frame",
		"Loads a specific grasp animation frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(UpdateFrameCmd, "Update Frame",
		"Updates the loaded frame with the new data", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(NextFrameCmd, "Goto Next Frame",
		"Load next frame from the animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PrevFrameCmd, "Goto Previous Frame",
		"Load previous frame from the animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(HelpEditGraspCmd, "Help",
		"Help for editing an existing grasp animation", EUserInterfaceActionType::Button, FInputChord());
}
#undef LOCTEXT_NAMESPACE