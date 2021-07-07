// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "MCGripperControlType.h"
#include "MCParallelGripperController.generated.h"

/**
 * Controller mapping the input from the user to the parallel gripper
 */
UCLASS()
class UMCParallelGripperController : public UObject
{
	GENERATED_BODY()
	
public:
	// Default constructor
	UMCParallelGripperController();

	// Initialize controller
	void Init(EMCGripperControlType ControlType,
		const FName& InputAxisName,
		UPhysicsConstraintComponent* LeftFingerConstraint,
		UPhysicsConstraintComponent* RightFingerConstraint,
		float InP, float InI, float InD, float InMax);

private:
	// Bind user inputs
	void SetupInputBindings(const FName& InputAxisName);

	// Setup the controller for linear drive (PD controller)
	// force = spring * (targetPosition - position) + damping * (targetVelocity - velocity)
	void SetupLinearDrive(float Spring, float Damping, float ForceLimit);

	// Update function bound to the input
	void Update(float Value);

	/* Update function bindings */
	// Function pointer type for calling the correct update function
	typedef void(UMCParallelGripperController::*UpdateFunctionPointerType)(float);

	// Function pointer for left/right update
	UpdateFunctionPointerType UpdateFunctionPointer;

	/* Default update functions */
	void Update_NONE(float Value);

	/* Update function for the linear driver */
	void Update_LinearDriver_X(float Value);
	void Update_LinearDriver_Y(float Value);
	void Update_LinearDriver_Z(float Value);

private:
	// Left finger constraint
	UPhysicsConstraintComponent* LeftConstraint;

	// Left finger constraint
	UPhysicsConstraintComponent* RightConstraint;

	// Left finger movement limit
	float LeftLimit;

	// Right finger movement limit
	float RightLimit;
};
