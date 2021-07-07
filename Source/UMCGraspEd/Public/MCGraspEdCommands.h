// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen

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
	TSharedPtr<FUICommandInfo> SaveMeshInitialStateCmd;
	TSharedPtr<FUICommandInfo> AddFrameCmd;
	TSharedPtr<FUICommandInfo> SaveAnimCmd;
	TSharedPtr<FUICommandInfo> ClearAllCmd;
	TSharedPtr<FUICommandInfo> HelpNewGraspCmd;

	TSharedPtr<FUICommandInfo> LoadFrameCmd;
	TSharedPtr<FUICommandInfo> UpdateFrameCmd;
	TSharedPtr<FUICommandInfo> NextFrameCmd;
	TSharedPtr<FUICommandInfo> PrevFrameCmd;
	TSharedPtr<FUICommandInfo> HelpEditGraspCmd;
};
