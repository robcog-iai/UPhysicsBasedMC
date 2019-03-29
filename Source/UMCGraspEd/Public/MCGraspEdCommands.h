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
	TSharedPtr<FUICommandInfo> CreateGraspAnim;
	TSharedPtr<FUICommandInfo> LoadGraspAnim;
	TSharedPtr<FUICommandInfo> SaveGraspPosition;
	TSharedPtr<FUICommandInfo> EditGraspPosition;
	TSharedPtr<FUICommandInfo> ShowNextFrame;
	TSharedPtr<FUICommandInfo> ShowPreviousFrame;
	TSharedPtr<FUICommandInfo> DiscardNewGraspAnim;
	TSharedPtr<FUICommandInfo> StartCreatingGrasp;
};
