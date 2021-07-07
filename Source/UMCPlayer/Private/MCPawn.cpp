// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCPawn.h"

// Sets default values
AMCPawn::AMCPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Auto possess player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create root component
	MCRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MCRoot"));
	SetRootComponent(MCRoot);

	// Create camera component
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	if (VRCamera)
	{
		VRCamera->SetWorldScale3D(FVector(0.25));
	}
#endif // WITH_EDITORONLY_DATA
}
