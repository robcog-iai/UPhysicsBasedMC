// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
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
	// Initialize controller
	void Init(EMCGripperControlType ControlType,
		UPhysicsConstraintComponent* LeftFingerConstraint,
		UPhysicsConstraintComponent* RightFingerConstraint);

private:
	// Bind user inputs
	void SetupInputBindings(UInputComponent* InIC);

	// Update function bound to the left input
	void UpdateLeft(const float Value);

	// Update function bound to the right input
	void UpdateRight(const float Value);
};
