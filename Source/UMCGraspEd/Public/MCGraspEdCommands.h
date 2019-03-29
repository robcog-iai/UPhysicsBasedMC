// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MCGraspEdStyle.h"
/**
 * 
 */
class UMCGraspEdCommands : public TCommands<UMCGraspEdCommands>
{
public:
	UMCGraspEdCommands() : TCommands<UMCGraspEdCommands>(
		TEXT("Grasping"), // Context name for fast lookup
		NSLOCTEXT("Contexts", "Grasping", "Grasping Plugin"), // Localized context name
		NAME_None, // Parent
		UMCGraspEdStyle::GetStyleSetName()// Icon Style Set
		) {}

	virtual void RegisterCommands() override;

	//The entries for the drop down menu.
	TSharedPtr<FUICommandInfo> ShowCreateHelp;
	TSharedPtr<FUICommandInfo> ShowEditHelp;
	TSharedPtr<FUICommandInfo> CreateGraspingStyle;
	TSharedPtr<FUICommandInfo> LoadGraspingStyle;
	TSharedPtr<FUICommandInfo> SaveGraspingPosition;
	TSharedPtr<FUICommandInfo> EditGraspingPosition;
	TSharedPtr<FUICommandInfo> ShowNextEpisode;
	TSharedPtr<FUICommandInfo> ShowPreviousEpisode;
	TSharedPtr<FUICommandInfo> DiscardNewGraspingStyle;
	TSharedPtr<FUICommandInfo> StartCreatingGrasp;
};
