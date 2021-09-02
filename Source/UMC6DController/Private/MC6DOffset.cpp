// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DOffset.h"
#if WITH_EDITORONLY_DATA
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#endif // WITH_EDITORONLY_DATA

// Sets default values for this component's properties
UMC6DOffset::UMC6DOffset()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	// Create and position a mesh component so we can see where our sphere is
	ControllerVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SLControllerVisual"));
	if (ControllerVisual)
	{
		ControllerVisual->SetupAttachment(this);
		static ConstructorHelpers::FObjectFinder<UStaticMesh> ControllerVisualAsset(TEXT("/Engine/VREditor/Devices/Vive/VivePreControllerMesh.VivePreControllerMesh"));
		if (ControllerVisualAsset.Succeeded())
		{
			ControllerVisual->SetStaticMesh(ControllerVisualAsset.Object);
			ControllerVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ControllerVisual->SetGenerateOverlapEvents(false);
			ControllerVisual->SetHiddenInGame(true);
		}
		ControllerVisual->SetHiddenInGame(true);
	}
#endif // WITH_EDITORONLY_DATA
}

