// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "PIDController3D.h"
#include "MC6DControlType.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

/**
 * 6D controller update callbacks
 */
struct FMC6DController
{
public:
	// Default constructor
	FMC6DController();

	// Destructor
	~FMC6DController() = default;

	// Init as skeletal mesh
	void Init(USceneComponent* InTarget,
		USkeletalMeshComponent* InSelfAsSkeletalMesh,
		bool bApplyToAllBodies,
		EMC6DControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot);

	// Init as skeletal mesh with an offset
	void Init(USceneComponent* InTarget,
		USkeletalMeshComponent* InSelfAsSkeletalMesh,
		bool bApplyToAllBodies,
		EMC6DControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Init as static mesh
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMC6DControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot);

	// Init as static mesh with an offset
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMC6DControlType ControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Reset the location pid controller
	void ResetLoc(float P, float I, float D, float Max, bool bClearErrors = true);

	// Reset the rotation pid controller
	void ResetRot(float P, float I, float D, float Max, bool bClearErrors = true);

	// Call the update function pointer
	void Update(float DeltaTime);

#if UMC_WITH_CHART
	// Get the chart data
	void GetDebugChartData(FVector& OutLocErr, FVector& OutLocPID, FVector& OutRotErr, FVector& OutRotPID);
#endif // UMC_WITH_CHART

private:
#if UMC_WITH_CHART
	// Set the chart data
	void SetDebugChartData(const FVector& InLocErr, const FVector& InLocPID, const FVector& InRotErr, const FVector& InRotPID);
#endif // UMC_WITH_CHART

	// Get the location delta (error)
	FVector GetRotationDelta(const FQuat& From, const FQuat& To);

private:
#if UMC_WITH_CHART
	// Cached data for chart visualization
	FVector LocErr;
	FVector LocPID;
	FVector RotErr;
	FVector RotPID;
#endif // UMC_WITH_CHART

	// Target (goal) component (to which transform to move to)
	USceneComponent* TargetSceneComp;

	// Relative offset to target (goal)
	FTransform LocalTargetOffset;

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
	typedef void(FMC6DController::*UpdateFunctionPointerType)(float);

	// Function pointer for update
	UpdateFunctionPointerType UpdateFunctionPointer;

	// Default update function
	void Update_NONE(float DeltaTime);

	// Skeletal updates
	void Update_Skel_Position(float DeltaTime);
	void Update_Skel_Velocity(float DeltaTime);
	void Update_Skel_Impulse(float DeltaTime);
	void Update_Skel_Acceleration(float DeltaTime);
	void Update_Skel_Force(float DeltaTime);

	// Skeletal updates with offset
	void Update_Skel_Position_Offset(float DeltaTime);
	void Update_Skel_Velocity_Offset(float DeltaTime);
	void Update_Skel_Impulse_Offset(float DeltaTime);
	void Update_Skel_Acceleration_Offset(float DeltaTime);
	void Update_Skel_Force_Offset(float DeltaTime);

	// Static mesh updates
	void Update_Static_Position(float DeltaTime);
	void Update_Static_Velocity(float DeltaTime);
	void Update_Static_Impulse(float DeltaTime);
	void Update_Static_Acceleration (float DeltaTime);
	void Update_Static_Force(float DeltaTime);

	// Static mesh updates with offset
	void Update_Static_Position_Offset(float DeltaTime);
	void Update_Static_Velocity_Offset(float DeltaTime);
	void Update_Static_Impulse_Offset(float DeltaTime);
	void Update_Static_Acceleration_Offset(float DeltaTime);
	void Update_Static_Force_Offset(float DeltaTime);
};
