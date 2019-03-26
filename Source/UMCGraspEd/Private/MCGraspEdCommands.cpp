// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdCommands.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

void UMCGraspEdCommands::RegisterCommands() {
	// Register all the UI elements

	UI_COMMAND(CreateGraspingStyle, "Create new grasping style out of saved episodes",
		"Create new grasping style out of your saved episodes", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SaveGraspingPosition, "Save current hand position as episode",
		"Save the hand position as an episode", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(DiscardNewGraspingStyle, "Discard all saved episodes",
		"Discard all newly saved episodes", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(LoadGraspingStyle, "Overwrite loaded episode",
		"Overwrite loaded episode with the hand position currently displayed", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(EditGraspingPosition, "Load grasping style",
		"Load a grasping style at a specific step", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowNextEpisode, "Show next episode",
		"Show next step", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowPreviousEpisode, "Show previous episode",
		"Show previous step", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowCreateHelp, "Show help",
		"create help", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ShowEditHelp, "Show help",
		"edit help", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE