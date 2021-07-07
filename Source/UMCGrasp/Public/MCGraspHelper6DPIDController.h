// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "EngineMinimal.h"
#include "MCPIDController3D.h"
#include "MCGraspHelper6DPIDController.generated.h"

// Forward declarations
class USkeletalMeshComponent;
class UStaticMeshComponent;

/**
* Control type
*	Position - teleport with physics on to the given transformation
*	Velocity - set velocity directly: distance(rad) / time, the effect is mass independent
*	Acceleration - set acceleration directly: distance(rad) / time^2, the effect is independent
*	Force - set force(torque): mass * distance(rad) / time^2
*	Impulse - set impulse: mass * distance(rad) / time
*/
UENUM()
enum class EMCGraspHelp6DControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Position				UMETA(DisplayName = "Position"),
	Velocity		 		UMETA(DisplayName = "Velocity"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Force					UMETA(DisplayName = "Force/Torque"),
	Impulse					UMETA(DisplayName = "Impulse"),
};

/**
 * 6D controller update callbacks
 */
USTRUCT(/*BlueprintType*/)
struct FMCGraspHelper6DPIDController
{
	GENERATED_BODY()

public:
	// Default constructor
	FMCGraspHelper6DPIDController();

	// Destructor
	~FMCGraspHelper6DPIDController() = default;

	// Init as static mesh with an offset
	void Init(USceneComponent* InTarget,
		UStaticMeshComponent* InSelfAsStaticMesh,
		EMCGraspHelp6DControlType LocControlType,
		float PLoc, float ILoc, float DLoc, float MaxLoc,
		EMCGraspHelp6DControlType RotControlType,
		float PRot, float IRot, float DRot, float MaxRot,
		FTransform InOffset);

	// Clear the update function
	void Clear();

	// Reset the location pid controller
	void ResetLoc(float P, float I, float D, float Max, bool bClearErrors = true);

	// Reset the rotation pid controller
	void ResetRot(float P, float I, float D, float Max, bool bClearErrors = true);

	// Call the update function pointer
	void UpdateController(float DeltaTime);

	// Get the location delta (error)
	FVector GetRotationDelta(const FQuat& From, const FQuat& To);

private:
	// Target (goal) component (to which transform to move to)
	USceneComponent* TargetSceneComp;

	// Relative offset to target (goal)
	FTransform LocalTargetOffset;

	// Self as static mesh (from which transform to move away)
	UStaticMeshComponent* SelfAsStaticMeshComp;

	// Location pid controller
	FMCPIDController3D PIDLoc;

	// Rotation pid controller
	FMCPIDController3D PIDRot;

	/* Update function bindings */
	// Function pointer type for calling the correct update function
	typedef void(FMCGraspHelper6DPIDController::*UpdateFunctionPointerType)(float);

	// Function pointer for location update
	UpdateFunctionPointerType LocUpdateFunctionPointer;

	// Function pointer for rotation update
	UpdateFunctionPointerType RotUpdateFunctionPointer;

	// Default update function
	void Loc_Update_NONE(float DeltaTime);
	void Rot_Update_NONE(float DeltaTime);

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
