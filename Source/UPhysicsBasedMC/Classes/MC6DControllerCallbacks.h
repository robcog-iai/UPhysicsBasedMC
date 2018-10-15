// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "PIDController3D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

/**
 * 6D controller update callbacks
 */
struct FMC6DControllerCallbacks
{
public:
	// Default constructor
	FMC6DControllerCallbacks();

	// Destructor
	~FMC6DControllerCallbacks();

	// Init as skeletal mesh
	void Init(USceneComponent* InTarget,
		USkeletalMeshComponent* InSelfAsSkeletalMesh,
		bool bApplyToAllBodies,
		EMCControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot);

	// Init as skeletal mesh with an offset
	void Init(USceneComponent* InTarget,
		USkeletalMeshComponent* InSelfAsSkeletalMesh,
		bool bApplyToAllBodies,
		EMCControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Init as static mesh
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMCControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot);

	// Init as static mesh with an offset
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMCControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Update
	void Update(float DeltaTime);

private:
	// Target (goal) component (to which transform to move to)
	USceneComponent* TargetSceneComp;

	// Offset to target (goal)
	FTransform TargetOffset;

	// Self as skeletal mesh (from which transform to move away)
	USkeletalMeshComponent* SelfAsSkeletalMeshComp;

	// Flag to apply the controller on every body of the skeletal mesh
	bool bApplyToAllChildBodies;

	// Self as static mesh (from which transform to move away)
	UStaticMeshComponent* SelfAsStaticMeshComp;

	// Location pid controller
	FPIDController3D PIDLoc;

	// Rotation pid controller
	FPIDController3D PIDRot;

	/* Update function bindings */
	// Function pointer type for calling the correct update function
	typedef void(FMC6DControllerCallbacks::*UpdateFunctionPointerType)(float);

	// Function pointer for update
	UpdateFunctionPointerType UpdateFunctionPointer;

	// Default update function
	void Update_NONE(float DeltaTime);

	// Skeletal updates
	void Update_Skel_Position(float DeltaTime);

	// Skeletal updates with offset
	void Update_Skel_Position_Offset(float DeltaTime);

	// Static mesh updates
	void Update_Static_Position(float DeltaTime);

	// Static mesh updates with offset
	void Update_Static_Position_Offset(float DeltaTime);
};
