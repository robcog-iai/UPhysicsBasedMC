// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PIDController3D.h"
#include "MCControlType.h"
#include "MC3DLocationController.generated.h"

// Forward declaration
class UMC6DControllerTarget;

/**
 * 3D location controller
 */
UCLASS()
class UPHYSICSBASEDMC_API UMC3DLocationController : public UObject
{
	GENERATED_BODY()

public:
	// Constructor
	UMC3DLocationController();

	// Destructor
	~UMC3DLocationController();

	// Init with skeletal mesh
	void Init(UMC6DControllerTarget* InMC,
		USkeletalMeshComponent* SkeletalMeshComp,
		FTransform InOffset,
		EMCControlType ControlType,
		float P, float I, float D, float Max,
		bool bApplyToAllChildBodies);

	// Init with static mesh
	void Init(UMC6DControllerTarget* InMCParent,
		UStaticMeshComponent* StaticMeshComp,
		FTransform InOffset,
		EMCControlType ControlType,
		float P, float I, float D, float Max,
		bool bApplyToAllChildBodies);

	// Update
	void Update(float DeltaTime);

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
	FVector Offset;

	// Function pointer type for calling the correct update function
	typedef void(UMC3DLocationController::*UpdateFunctionPointerType)(float);

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
