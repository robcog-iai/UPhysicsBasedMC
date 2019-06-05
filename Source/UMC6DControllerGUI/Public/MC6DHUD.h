// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MC6DHUD.generated.h"

/**
 * 
 */
UCLASS()
class UMC6DCONTROLLERGUI_API AMC6DHUD : public AHUD
{
	GENERATED_BODY()

protected:
	// Called when the games starts
	virtual void BeginPlay() override;

public:
	TSharedPtr<class SMC6DWidget> MC6DWidget;
#if UMC_WITH_KANTAN
	TSharedPtr<class SKantanTimeSeriesPlot> TimeSeriesPlot;
#endif // UMC_WITH_KANTAN
};
