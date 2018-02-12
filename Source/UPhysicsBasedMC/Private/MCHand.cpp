// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCHand.h"

// Sets default values
UMCHand::UMCHand()
{
	// Disable ticking in the editor
	bTickInEditor = false;

	// Default values for the skeletal mesh
	SetSimulatePhysics(true);
	SetEnableGravity(false);
	SetCollisionProfileName(TEXT("BlockAll"));
	bGenerateOverlapEvents = true;
	
	/* Control parameters */
	// Movement controller location PID default parameters
	MovementController.LocationPIDController.P = 250.0;
	MovementController.LocationPIDController.I = 0.0f;
	MovementController.LocationPIDController.D = 50.0f;
	MovementController.LocationPIDController.MaxOutAbs = 1500.f;

	// Movement controller rotation PID default parameters
	MovementController.RotationPIDController.P = 100.f;
	MovementController.RotationPIDController.I = 0.0f;
	MovementController.RotationPIDController.D = 0.0f;
	MovementController.RotationPIDController.MaxOutAbs = 1500.f;

	// Default control type
	MovementController.LocationControlType = EMCLocationControlType::Acceleration;
	MovementController.RotationControlType = EMCRotationControlType::Velocity;
}

// Called when the game starts or when spawned
void UMCHand::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default, enable it in Init() (if called externally)
	SetComponentTickEnabled(false);
}

// Called every frame, used for motion control
void UMCHand::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the movement control of the hand
	MovementController.Update(DeltaTime);
}

// Init hand with the motion controllers
void UMCHand::Init(UMotionControllerComponent* InMC)
{
	// Init the movement controller
	MovementController.Init(this, InMC);
			
	// Enable Tick
	SetComponentTickEnabled(true);
}