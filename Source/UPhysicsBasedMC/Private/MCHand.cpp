// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCHand.h"

// Sets default values
UMCHand::UMCHand(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Disable ticking in the editor
	bTickInEditor = false;

	// Default values for the skeletal mesh
	SetSimulatePhysics(true);
	SetEnableGravity(false);
	SetCollisionProfileName(TEXT("BlockAll"));
	bGenerateOverlapEvents = true;
	
	// Create the movement controller component
	MovementController = ObjectInitializer.CreateDefaultSubobject<UMCMovementController6D>(
		this, FName(*GetName().Append(TEXT("_MovementController"))));

	// Create grasp component
	GraspController = ObjectInitializer.CreateDefaultSubobject<UMCGraspController>(
		this, FName(*GetName().Append(TEXT("_GraspController"))));

	// Create fixation grasp component
	FixationGraspController = ObjectInitializer.CreateDefaultSubobject<UMCFixationGraspController>(
		this, FName(*GetName().Append(TEXT("_FixationGraspController"))));
	FixationGraspController->SetupAttachment(this);

	// Enable fixation grasp by default
	bEnableFixationGrasp = true;
}

// Called when the game starts or when spawned
void UMCHand::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default, enable it if Init() is called
	SetComponentTickEnabled(false);
}

// Called every frame, used for motion control
void UMCHand::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the movement control of the hand
	MovementController->Update(DeltaTime);
}

// Init hand with the motion controllers
void UMCHand::Init(UMotionControllerComponent* InMC)
{
	// Init the movement controller
	MovementController->Init(this, InMC);	

	// Init the grasp controller
	GraspController->Init(this, InMC);

	// Init the fixation grasp controller
	if (bEnableFixationGrasp)
	{
		FixationGraspController->Init(this, InMC);
	}
	else
	{
		FixationGraspController->DestroyComponent();
	}

	// Enable Tick
	SetComponentTickEnabled(true);
}