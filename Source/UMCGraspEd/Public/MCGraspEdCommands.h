// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

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
		NSLOCTEXT("Contexts", "AnimGrasp", "Animation Grasp Plugin"), // Localized context name
		NAME_None, // Parent
		UMCGraspEdStyle::GetStyleSetName()// Icon Style Set
		) {}

	// Describe and instantiate the commands 
	virtual void RegisterCommands() override;

	// The commands for the drop down menu entries
	TSharedPtr<FUICommandInfo> InitGraspCmd;
	TSharedPtr<FUICommandInfo> ShowHelpNewGraspCmd;
	TSharedPtr<FUICommandInfo> ShowEditGraspAnimHelpCommand;
	TSharedPtr<FUICommandInfo> SaveAnimCmd;
	TSharedPtr<FUICommandInfo> LoadGraspAnimFrameCommand;
	TSharedPtr<FUICommandInfo> SaveFrameCmd;
	TSharedPtr<FUICommandInfo> UpdateFraspAnimFrame;
	TSharedPtr<FUICommandInfo> GotoNextGraspAnimFrameCommand;
	TSharedPtr<FUICommandInfo> GotoPrevGraspAnimFrameCommand;
	TSharedPtr<FUICommandInfo> ClearAnimCmd;
};
