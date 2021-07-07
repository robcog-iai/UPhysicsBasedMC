// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCParallelGripperController.h"

// Default constructor
UMCParallelGripperController::UMCParallelGripperController()
{
	// Bind update function pointer  with default
	UpdateFunctionPointer = &UMCParallelGripperController::Update_NONE;
}

// Init controller
void UMCParallelGripperController::Init(EMCGripperControlType ControlType,
	const FName& InputAxisName,
	UPhysicsConstraintComponent* LeftFingerConstraint,
	UPhysicsConstraintComponent* RightFingerConstraint,
	float InP, float InI, float InD, float InMax)
{
	if (LeftFingerConstraint == nullptr || RightFingerConstraint == nullptr)
	{
		return;
	}

	// Set member values
	LeftConstraint = LeftFingerConstraint;
	RightConstraint = RightFingerConstraint;
	LeftLimit = LeftConstraint->ConstraintInstance.GetLinearLimit();
	RightLimit = RightConstraint->ConstraintInstance.GetLinearLimit();

	// Set the user input bindings
	UMCParallelGripperController::SetupInputBindings(InputAxisName);

	// Initialize control types
	switch (ControlType)
	{
	case EMCGripperControlType::Position:
		// TODO not implemented
		break;
	case EMCGripperControlType::LinearDrive:
		UMCParallelGripperController::SetupLinearDrive(InP, InD, InMax);
		break;
	case EMCGripperControlType::Acceleration:
		// TODO not implemented
		break;
	case EMCGripperControlType::Force:
		// TODO not implemented
		break;
	}
}

// Bind user input to function
void UMCParallelGripperController::SetupInputBindings(const FName& InputAxisName)
{
	// Set user input bindings
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAxis(InputAxisName, this, &UMCParallelGripperController::Update);
		}
	}
}

// Setup the controller for linear drive (PD controller)
// force = spring * (targetPosition - position) + damping * (targetVelocity - velocity)
void UMCParallelGripperController::SetupLinearDrive(float Spring, float Damping, float ForceLimit)
{
	// Set linear driver parameters, it is a proportional derivative (PD) drive, 
	// where force = spring * (targetPosition - position) + damping * (targetVelocity - velocity)
	LeftConstraint->SetLinearDriveParams(Spring, Damping, ForceLimit);
	RightConstraint->SetLinearDriveParams(Spring, Damping, ForceLimit);

	// Set axis movement and bind update function
	if (LeftConstraint->ConstraintInstance.GetLinearXMotion() == ELinearConstraintMotion::LCM_Limited &&
		RightConstraint->ConstraintInstance.GetLinearXMotion() == ELinearConstraintMotion::LCM_Limited)
	{
		LeftConstraint->SetLinearPositionDrive(true, false, false);
		RightConstraint->SetLinearPositionDrive(true, false, false);
		UpdateFunctionPointer = &UMCParallelGripperController::Update_LinearDriver_X;
	}
	else if (LeftConstraint->ConstraintInstance.GetLinearYMotion() == ELinearConstraintMotion::LCM_Limited &&
		RightConstraint->ConstraintInstance.GetLinearYMotion() == ELinearConstraintMotion::LCM_Limited)
	{
		LeftConstraint->SetLinearPositionDrive(false, true, false);
		RightConstraint->SetLinearPositionDrive(false, true, false);
		UpdateFunctionPointer = &UMCParallelGripperController::Update_LinearDriver_Y;
	}
	else if (LeftConstraint->ConstraintInstance.GetLinearZMotion() == ELinearConstraintMotion::LCM_Limited &&
		RightConstraint->ConstraintInstance.GetLinearZMotion() == ELinearConstraintMotion::LCM_Limited)
	{
		LeftConstraint->SetLinearPositionDrive(false, false, true);
		RightConstraint->SetLinearPositionDrive(false, false, true);
		UpdateFunctionPointer = &UMCParallelGripperController::Update_LinearDriver_Z;
	}
}

// Update function bound to the input
void UMCParallelGripperController::Update(float Value)
{
	(this->*UpdateFunctionPointer)(Value);
}

/* Default update function */
void UMCParallelGripperController::Update_NONE(float Value)
{
}

/* Update function for the linear driver */
void UMCParallelGripperController::Update_LinearDriver_X(float Value)
{
	// Value is normalized x=[0,1]
	// Mapping function is:
	// f(x) = (1-x)*MinLim + x*MaxLim; 
	// since the limits are symmetrical, MinLim = -MaxLim, the function becomes:
	// f(x) = (1-x)*-MaxLim + x*MaxLim => f(x) = (x-1)MaxLim + x*MaxLim

	// Left target becomes
	const float LeftTarget = ((Value - 1.f) * LeftLimit) + (Value * LeftLimit);
	
	// Right target is mirrored, hence a multiplication by -1 is needed
	const float RightTarget = ((1.f - Value) * RightLimit) - (Value * RightLimit);

	// Apply target command
	LeftConstraint->SetLinearPositionTarget(FVector(LeftTarget, 0.f, 0.f));
	RightConstraint->SetLinearPositionTarget(FVector(RightTarget, 0.f, 0.f));
}

void UMCParallelGripperController::Update_LinearDriver_Y(float Value)
{
	// Value is normalized x=[0,1]
	// Mapping function is:
	// f(x) = (1-x)*MinLim + x*MaxLim; 
	// since the limits are symmetrical, MinLim = -MaxLim, the function becomes:
	// f(x) = (1-x)*-MaxLim + x*MaxLim => f(x) = (x-1)MaxLim + x*MaxLim

	// Left target becomes
	const float LeftTarget = ((1.f - Value) * LeftLimit) - (Value * LeftLimit);

	// Right target is mirrored, hence a multiplication by -1 is needed
	const float RightTarget = ((Value - 1.f) * RightLimit) + (Value * RightLimit);

	// Apply target command
	LeftConstraint->SetLinearPositionTarget(FVector(0.f, LeftTarget, 0.f));
	RightConstraint->SetLinearPositionTarget(FVector(0.f, RightTarget, 0.f));
}

void UMCParallelGripperController::Update_LinearDriver_Z(float Value)
{
	// Value is normalized x=[0,1]
	// Mapping function is:
	// f(x) = (1-x)*MinLim + x*MaxLim; 
	// since the limits are symmetrical, MinLim = -MaxLim, the function becomes:
	// f(x) = (1-x)*-MaxLim + x*MaxLim => f(x) = (x-1)MaxLim + x*MaxLim

	// Left target becomes
	const float LeftTarget = ((Value - 1.f) * LeftLimit) + (Value * LeftLimit);

	// Right target is mirrored, hence a multiplication by -1 is needed
	const float RightTarget = ((1.f - Value) * RightLimit) - (Value * RightLimit);

	// Apply target command
	LeftConstraint->SetLinearPositionTarget(FVector(0.f, 0.f, LeftTarget));
	RightConstraint->SetLinearPositionTarget(FVector(0.f, 0.f, RightTarget));
}
