// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UMC6DCONTROLLERGUI_API SMC6DWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMC6DWidget){}

	// Creates _OwnerHUDArg variable
	SLATE_ARGUMENT(TWeakObjectPtr<class AMC6DHUD>, OwnerHUDArg);

	SLATE_END_ARGS()

	// Constructs the widget with the given arguments
	void Construct(const FArguments& InArgs);

private:
	// Pointer to the parent HUD
	TWeakObjectPtr<class AMC6DHUD> OwnerHUD;
};
