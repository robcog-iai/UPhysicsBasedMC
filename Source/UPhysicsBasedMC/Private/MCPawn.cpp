// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCPawn.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "XRMotionControllerBase.h"

// Sets default values
AMCPawn::AMCPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Possess player automatically
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Display MC meshes by default
	bVisualizeMCMeshes = true;

	// Crate MC root
	MCRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MCRoot"));
	//MCRoot->SetupAttachment(GetRootComponent());
	RootComponent = MCRoot;
	
	// Create camera component
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(MCRoot);
		
	// Create the right motion controller
	MCLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MCLeft"));
	MCLeft->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	MCLeft->SetupAttachment(MCRoot);
	
	// Create the left motion controller
	MCRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MCRight"));
	MCRight->MotionSource = FXRMotionControllerBase::RightHandSourceId; 
	MCRight->SetupAttachment(MCRoot);

	// Create Left MC Hand Component
	MCHandLeft = CreateDefaultSubobject<UMCHand>(TEXT("LeftHand"));
	MCHandLeft->SetupAttachment(GetRootComponent());

	// Create Right MC Hand Component
	MCHandRight = CreateDefaultSubobject<UMCHand>(TEXT("RightHand"));
	MCHandRight->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AMCPawn::BeginPlay()
{
	Super::BeginPlay();

	// MC meshes visualization
	MCLeft->bDisplayDeviceModel = bVisualizeMCMeshes;
	MCRight->bDisplayDeviceModel = bVisualizeMCMeshes;

	// Init MC Hands
	MCHandLeft->Init(MCLeft);
	MCHandRight->Init(MCRight);

	// Disable tick
	SetActorTickEnabled(false);

	// Turns hands invisible for any clients and instead turns poseable meshes visible
	if (this->Role >= ROLE_Authority) {
		MCHandLeft->SetVisibility(true, false);
		MCHandRight->SetVisibility(true, false);
		MCHandLeft->PoseableMesh->SetVisibility(false, false);
		MCHandRight->PoseableMesh->SetVisibility(false, false);
		MCHandLeft->bIsServer = true;
		MCHandRight->bIsServer = true;
	}
	else {
		MCHandLeft->SetVisibility(false, false);
		MCHandRight->SetVisibility(false, false);
		MCHandLeft->PoseableMesh->SetVisibility(true, false);
		MCHandRight->PoseableMesh->SetVisibility(true, false);
		MCHandLeft->bIsServer = false;
		MCHandRight->bIsServer = false;
	}
}

// Called every frame
void AMCPawn::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Disable me"), *FString(__FUNCTION__));
}

// Called to bind functionality to input
void AMCPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
