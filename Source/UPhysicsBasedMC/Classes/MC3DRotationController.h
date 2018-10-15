// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/NoExportTypes.h"
#include "PIDController3D.h"
#include "MCControlType.h"
#include "MC3DRotationController.generated.h"

// Forward declaration
class UMC6DControllerTarget;
class UMC6DControllerOffset;

/**
 * 3D rotation controller
 */
UCLASS()
class UMC3DRotationController : public UObject
{
	GENERATED_BODY()
	
public:
	// Constructor
	UMC3DRotationController();
	
	// Destructor
	~UMC3DRotationController();

	// Update
	void Update(float DeltaTime);

	// Init with skeletal mesh
	void Init(UMC6DControllerTarget* InMCParent,
		USkeletalMeshComponent* SkeletalMeshComp,
		FTransform Offset,
		EMCControlType ControlType,
		float P, float I, float D, float Max,
		bool bApplyToAllChildBodies);

	// Init with static mesh
	void Init(UMC6DControllerTarget* InMCParent,
		UStaticMeshComponent* StaticMeshComp,
		FTransform Offset,
		EMCControlType ControlType,
		float P, float I, float D, float Max,
		bool bApplyToAllChildBodies);

private:
	// PID Controller
	FPIDController3D PIDController;

	// Motion controller parent (Target)
	UMC6DControllerTarget* MC;

	// Owner skeletal mesh component (if controlling a skeletal mesh)
	USkeletalMeshComponent* SkeletalMesh;

	// Owner static mesh component (if controlling a static mesh)
	UStaticMeshComponent* StaticMesh;

	// Offset to apply
	FQuat Offset;

	// Function pointer type for calling the correct update function
	typedef void(UMC3DRotationController::*UpdateFunctionPointerType)(float);

	// Function pointer for update
	UpdateFunctionPointerType UpdateFunctionPointer;

	// Update function types
	/* Empty / default function */
	void Update_NONE(float DeltaTime) {};

	/* Skeletal mesh update functions */
	void Update_Skel_Position(float DeltaTime);
	void Update_Skel_Velocity(float DeltaTime);
	void Update_Skel_Acceleration(float DeltaTime);
	void Update_Skel_Force(float DeltaTime);
	void Update_Skel_Impulse(float DeltaTime);

	// With offset
	void Update_Skel_Position_Offset(float DeltaTime);
	void Update_Skel_Velocity_Offset(float DeltaTime);
	void Update_Skel_Acceleration_Offset(float DeltaTime);
	void Update_Skel_Force_Offset(float DeltaTime);
	void Update_Skel_Impulse_Offset(float DeltaTime);

	/* Static mesh update functions */
	void Update_Static_Position(float DeltaTime);
	void Update_Static_Velocity(float DeltaTime);
	void Update_Static_Acceleration(float DeltaTime);
	void Update_Static_Force(float DeltaTime);
	void Update_Static_Impulse(float DeltaTime);

	// With offset
	void Update_Static_Position_Offset(float DeltaTime);
	void Update_Static_Velocity_Offset(float DeltaTime);
	void Update_Static_Acceleration_Offset(float DeltaTime);
	void Update_Static_Force_Offset(float DeltaTime);
	void Update_Static_Impulse_Offset(float DeltaTime);
};
