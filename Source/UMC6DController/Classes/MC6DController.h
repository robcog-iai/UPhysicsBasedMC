// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "EngineMinimal.h"
#include "MCPIDController3D.h"
#include "MC6DControlType.h"
#include "MC6DController.generated.h"

// Forward declarations
class USkeletalMeshComponent;
class UStaticMeshComponent;

/**
 * 6D controller update callbacks
 */
USTRUCT(/*BlueprintType*/)
struct FMC6DController
{
	GENERATED_BODY()

public:
	// Default constructor
	FMC6DController();

	// Destructor
	~FMC6DController() = default;

	// Init as skeletal mesh
	void Init(USceneComponent* InTarget,
		USkeletalMeshComponent* InSelfAsSkeletalMesh,
		bool bApplyToAllBodies,
		EMC6DControlType LocControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		EMC6DControlType RotControlType,
		float PRot, float IRot, float DRot, float MaxRot);

	// Init as skeletal mesh with an offset
	void Init(USceneComponent* InTarget,
		USkeletalMeshComponent* InSelfAsSkeletalMesh,
		bool bApplyToAllBodies,
		EMC6DControlType LocControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		EMC6DControlType RotControlType,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Init as static mesh
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMC6DControlType LocControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		EMC6DControlType RotControlType,
		float PRot, float IRot, float DRot, float MaxRot);

	// Init as static mesh with an offset
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMC6DControlType LocControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		EMC6DControlType RotControlType,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Clear the update function
	void Clear();

	// Overwrite the update function to use bone location as target
	void OverwriteToUseBoneForTargetLocation(USkeletalMeshComponent* TargetSkeletalMeshComponent, FName TargetBoneName);

	// Reset the location pid controller
	void ResetLoc(float P, float I, float D, float Max, bool bClearErrors = true);

	// Reset the rotation pid controller
	void ResetRot(float P, float I, float D, float Max, bool bClearErrors = true);

	// Call the update function pointer
	void UpdateController(float DeltaTime);

#if UMC_WITH_CHART
	// Get the chart data
	void GetDebugChartData(FVector& OutLocErr, FVector& OutLocPID, FVector& OutRotErr, FVector& OutRotPID);
#endif // UMC_WITH_CHART

private:
#if UMC_WITH_CHART
	// Set the chart data
	void SetLocDebugChartData(const FVector& InLocErr, const FVector& InLocPID);

	// Set the rotation chart data
	void SetRotDebugChartData(const FVector& InRotErr, const FVector& InRotPID);
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

	// To overwrite location as a skeletal bone
	USkeletalMeshComponent* OverwriteTargetSkMC;
	
	// Use this bone as target location
	FName OverwriteTargetBoneName;

	// Used for direct transform access
	int32 OverwriteTargetBoneIndex;

	// True if the target location, can and should be overwritten by a bone location
	bool bOverwriteTargetLocation;

	// Relative offset to target (goal)
	FTransform LocalTargetOffset;

	// Self as skeletal mesh (from which transform to move away)
	USkeletalMeshComponent* SelfAsSkeletalMeshComp;

	// Flag to apply the controller on every body of the skeletal mesh
	bool bApplyToAllChildBodies;

	// Self as static mesh (from which transform to move away)
	UStaticMeshComponent* SelfAsStaticMeshComp;

	// Location pid controller
	FMCPIDController3D PIDLoc;

	// Rotation pid controller
	FMCPIDController3D PIDRot;

	/* Update function bindings */
	// Function pointer type for calling the correct update function
	typedef void(FMC6DController::*UpdateFunctionPointerType)(float);

	// Function pointer for location update
	UpdateFunctionPointerType LocUpdateFunctionPointer;

	// Function pointer for rotation update
	UpdateFunctionPointerType RotUpdateFunctionPointer;

	// Default update function
	void Loc_Update_NONE(float DeltaTime);
	void Rot_Update_NONE(float DeltaTime);

	/* Skeletal updates */
	// Loc
	void Loc_Update_Skel_Position(float DeltaTime);
	void Loc_Update_Skel_Velocity(float DeltaTime);
	void Loc_Update_Skel_Impulse(float DeltaTime);
	void Loc_Update_Skel_Acceleration(float DeltaTime);
	void Loc_Update_Skel_Force(float DeltaTime);
	// Rot
	void Rot_Update_Skel_Position(float DeltaTime);
	void Rot_Update_Skel_Velocity(float DeltaTime);
	void Rot_Update_Skel_Impulse(float DeltaTime);
	void Rot_Update_Skel_Acceleration(float DeltaTime);
	void Rot_Update_Skel_Force(float DeltaTime);

	/* Skeletal updates with offset */
	// Loc
	void Loc_Update_Skel_Position_Offset(float DeltaTime);
	void Loc_Update_Skel_Velocity_Offset(float DeltaTime);
	void Loc_Update_Skel_Impulse_Offset(float DeltaTime);
	void Loc_Update_Skel_Acceleration_Offset(float DeltaTime);
	void Loc_Update_Skel_Force_Offset(float DeltaTime);
	// Rot
	void Rot_Update_Skel_Position_Offset(float DeltaTime);
	void Rot_Update_Skel_Velocity_Offset(float DeltaTime);
	void Rot_Update_Skel_Impulse_Offset(float DeltaTime);
	void Rot_Update_Skel_Acceleration_Offset(float DeltaTime);
	void Rot_Update_Skel_Force_Offset(float DeltaTime);

	/* Static mesh updates */
	// Loc
	void Loc_Update_Static_Position(float DeltaTime);
	void Loc_Update_Static_Velocity(float DeltaTime);
	void Loc_Update_Static_Impulse(float DeltaTime);
	void Loc_Update_Static_Acceleration (float DeltaTime);
	void Loc_Update_Static_Force(float DeltaTime);
	// Rot
	void Rot_Update_Static_Position(float DeltaTime);
	void Rot_Update_Static_Velocity(float DeltaTime);
	void Rot_Update_Static_Impulse(float DeltaTime);
	void Rot_Update_Static_Acceleration(float DeltaTime);
	void Rot_Update_Static_Force(float DeltaTime);

	/* Static mesh updates with offset */
	// Loc
	void Loc_Update_Static_Position_Offset(float DeltaTime);
	void Loc_Update_Static_Velocity_Offset(float DeltaTime);
	void Loc_Update_Static_Impulse_Offset(float DeltaTime);
	void Loc_Update_Static_Acceleration_Offset(float DeltaTime);
	void Loc_Update_Static_Force_Offset(float DeltaTime);
	// Rot
	void Rot_Update_Static_Position_Offset(float DeltaTime);
	void Rot_Update_Static_Velocity_Offset(float DeltaTime);
	void Rot_Update_Static_Impulse_Offset(float DeltaTime);
	void Rot_Update_Static_Acceleration_Offset(float DeltaTime);
	void Rot_Update_Static_Force_Offset(float DeltaTime);
};
