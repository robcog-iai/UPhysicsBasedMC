// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCommands.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

void UMCGraspEdCommands::RegisterCommands() {
	// Register all the UI elements

	UI_COMMAND(StartCreatingGrasp, "Start",
		"Starts the process of creating a grasp animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SaveGraspPosition, "Add Frame",
		"Add current position to animation as a frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CreateGraspAnim, "Save",
		"Save to data asset", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(DiscardNewGraspAnim, "Clear All",
		"Discards all frames", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(LoadGraspAnim, "Load Grasp Animation",
		"Loads a graso animation at a specific frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(EditGraspPosition, "Update Frame",
		"Updates the loaded frame with current positions", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowNextFrame, "Goto Next Frame",
		"Goto Next Frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowPreviousFrame, "Goto Previous Frame",
		"Goto Previous Frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowCreateHelp, "Help",
		"create help", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowEditHelp, "Help",
		"edit help", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE