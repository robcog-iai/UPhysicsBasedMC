// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCommands.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

void UMCGraspEdCommands::RegisterCommands() {
	// Register all the UI elements

	UI_COMMAND(StartCreatingGrasp, "Start",
		"Starts the process of creating a grasp animation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SaveGraspingPosition, "Add Frame",
		"Add current position to animation as a frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CreateGraspingStyle, "Save",
		"Save to data asset", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(DiscardNewGraspingStyle, "Clear All",
		"Discards all frames", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(LoadGraspingStyle, "Load Grasp Animation",
		"Loads a graso animation at a specific frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(EditGraspingPosition, "Update Frame",
		"Updates the loaded frame with current positions", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowNextEpisode, "Goto Next Frame",
		"Goto Next Frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowPreviousEpisode, "Goto Previous Frame",
		"Goto Previous Frame", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowCreateHelp, "Help",
		"create help", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowEditHelp, "Help",
		"edit help", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE