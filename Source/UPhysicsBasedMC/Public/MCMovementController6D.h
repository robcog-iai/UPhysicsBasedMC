// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionControllerComponent.h"
#include "PIDController3D.h"
#include "MCMovementController6D.generated.h"

/**
* Location control type of the hands
*/
UENUM()
enum class EMCLocationControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Force					UMETA(DisplayName = "Force"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Impulse					UMETA(DisplayName = "Impulse"),
	Velocity	     		UMETA(DisplayName = "Velocity"),
	Position    			UMETA(DisplayName = "Position"),
};

/**
* Rotation control type of the hands
*/
UENUM()
enum class EMCRotationControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Torque					UMETA(DisplayName = "Torque"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Impulse					UMETA(DisplayName = "Impulse"),
	Velocity	     		UMETA(DisplayName = "Velocity"),
	Position    			UMETA(DisplayName = "Position"),
};

/**
 * 3D Movement controller of the hand
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCMovementController6D : public USceneComponent /*UObject*/ /*UActorComponent*/
{
	GENERATED_BODY()
	
public:
	// Constructor, set default values
	UMCMovementController6D();

	// Init hand with the motion controllers
	void Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC);

	// Update the movement
	void Update(const float DeltaTime);

	// Use scene component as a tracking offset
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	bool bUseTrackingOffset;

	//// Custom bone for tracking location
	//UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseTrackingOffset"))
	//USceneComponent* TrackingOffsetComp;
	
	//// Use custom bone as tracking location // TODO use a scene component to avoid moving bones rotation control
	//UPROPERTY(EditAnywhere, Category = "Movement Control")
	//bool bUseCustomBoneForTracking;
	//// Custom bone for tracking location
	//UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseCustomBoneForTracking"))
	//FName CustomBoneFName;


	/* Control */
	// Location PID controller
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	FPIDController3D LocationPIDController;

	// Rotation PID controller
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	FPIDController3D RotationPIDController;

	// Location controller type
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	EMCLocationControlType LocationControlType;

	// Rotation controller type
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	EMCRotationControlType RotationControlType;

private:
	// Skeletal mesh component of the hand
	USkeletalMeshComponent* HandSkelComp;

	// Motion controller to follow
	UMotionControllerComponent* MC;

	// Hand rotation offset for hand alignment
	FQuat HandRotationAlignmentOffset;

	// Control function pointer variable type
	typedef void(UMCMovementController6D::*MovementControlFuncPtrType)(float);

	// Function pointer for location movement control
	MovementControlFuncPtrType LocationControlFuncPtr;

	// Function pointer for rotation movement control
	MovementControlFuncPtrType RotationControlFuncPtr;

	// Location interaction functions types
	void LocationControl_None(float InDeltaTime);
	void LocationControl_ForceBased(float InDeltaTime);
	void LocationControl_AccelBased(float InDeltaTime);
	void LocationControl_AccelBased_Offset(float InDeltaTime);
	void LocationControl_ImpulseBased(float InDeltaTime);
	void LocationControl_VelBased(float InDeltaTime);
	void LocationControl_PosBased(float InDeltaTime);

	// Rotation interaction function types
	void RotationControl_None(float InDeltaTime);
	void RotationControl_TorqueBased(float InDeltaTime);
	void RotationControl_AccelBased(float InDeltaTime);
	void RotationControl_ImpulseBased(float InDeltaTime);
	void RotationControl_VelBased(float InDeltaTime);
	void RotationControl_VelBased_Offset(float InDeltaTime);
	void RotationControl_PosBased(float InDeltaTime);	
};
