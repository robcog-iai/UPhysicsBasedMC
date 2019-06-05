// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DHUD.h"
#include "MC6DWidget.h"
#include "Widgets/SWeakWidget.h"
#if UMC_WITH_KANTAN
#include "Charts/SKantanTimeSeriesPlot.h"
#endif // UMC_WITH_KANTAN

// Called when the game starts
void AMC6DHUD::BeginPlay()
{
	Super::BeginPlay();

	// Set the owner argument
	MC6DWidget = SNew(SMC6DWidget).OwnerHUDArg(this);

	GEngine->GameViewport->AddViewportWidgetContent(
		SNew(SWeakWidget).PossiblyNullContent(MC6DWidget.ToSharedRef()));
	MC6DWidget->SetVisibility(EVisibility::Visible);

#if UMC_WITH_KANTAN
	TimeSeriesPlot = SNew(SKantanTimeSeriesPlot);

	GEngine->GameViewport->AddViewportWidgetContent(
		SNew(SWeakWidget).PossiblyNullContent(TimeSeriesPlot.ToSharedRef()));
	TimeSeriesPlot->SetVisibility(EVisibility::Visible);
#endif // UMC_WITH_KANTAN
}