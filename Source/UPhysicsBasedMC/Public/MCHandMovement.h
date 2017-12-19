// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once


#include "EngineMinimal.h"
//#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "MotionControllerComponent.h"
#include "MCEnums.h"
#include "MCHandMovement.generated.h"

/**
*	Movement controller logic for the VR hands
*/
USTRUCT(BlueprintType)
struct UPHYSICSBASEDMC_API FMCHandMovement
{
	GENERATED_USTRUCT_BODY()

public:
	// Location / Rotation interaction type (movement type of the hand)
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	EMCLocationControlType LocationControlType;
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	EMCRotationControlType RotationControlType;

	// Left/Right hand location PID controller
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D LeftLocationPIDController;
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D RightLocationPIDController;

	// Left/Right hand rotation PID controller
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D LeftRotationPIDController;
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D RightRotationPIDController;

	// Default constructor (no initialization)
	FMCHandMovement() {	}

	// Init
	bool Init(UMotionControllerComponent* InLeftMC,
		UMotionControllerComponent* InRightMC,
		USkeletalMeshComponent* InLeftSkelMeshComp,
		USkeletalMeshComponent* InRightSkelMeshComp);

	// Set location PIDs
	void SetLocationPIDs(const float InLocP, const float InLocI = 0.f, const float InLocD = 0.f);

	// Set rotation PIDs
	void SetRotationPIDs(const float InRotP, const float InRotI = 0.f, const float InRotD = 0.f);

	// Set MC components
	void SetMCComponents(UMotionControllerComponent* InLeftMC, UMotionControllerComponent* InRightMC);

	// Set hand skeletal components
	void SetHandSkelComponents(USkeletalMeshComponent* InLeftSkelMeshComp, USkeletalMeshComponent* InRightSkelMeshComp);
	
	// Location update
	void LocationUpdate(float InDeltaTime);

	// Rotation update
	void RotationUpdate(float InDeltaTime);

private:
	// MC Components
	UMotionControllerComponent * LeftMC;
	UMotionControllerComponent* RightMC;

	// Hank skeletal mesh components
	USkeletalMeshComponent* LeftSkelMeshComp;
	USkeletalMeshComponent* RightSkelMeshComp;

	// Location / Rotation update function pointer variable type
	typedef void(FMCHandMovement::*LocationUpdateFunctionPtrType)(float);
	typedef void(FMCHandMovement::*RotationUpdateFunctionPtrType)(float);

	// Function pointer for the Location / Rotation interaction type
	LocationUpdateFunctionPtrType LocationUpdateFunctionPtr;
	RotationUpdateFunctionPtrType RotationUpdateFunctionPtr;

	// Bind location / rotation update function
	void BindLocationUpdateFunction();
	void BindRotationUpdateFunction();

	// Location interaction functions types
	void LocationUpdate_AccBased(float InDeltaTime);
	void LocationUpdate_VelBased(float InDeltaTime);

	// Rotation interaction function types
	void RotationUpdate_AccBased(float InDeltaTime);
	void RotationUpdate_VelBased(float InDeltaTime);
};

/* FMCHandMovement inline functions
*****************************************************************************/
// Init
bool FORCEINLINE FMCHandMovement::Init(
	UMotionControllerComponent* InLeftMC,
	UMotionControllerComponent* InRightMC,
	USkeletalMeshComponent* InLeftSkelMeshComp,
	USkeletalMeshComponent* InRightSkelMeshComp)
{
	// Init the PIDs
	LeftLocationPIDController.Init();
	RightLocationPIDController.Init();
	LeftRotationPIDController.Init();
	RightRotationPIDController.Init();

	UE_LOG(LogTemp, Error, TEXT("PIDS: \n Loc: \n \t Left: %f %f %f, \n \t Right: %f %f %f, \n Rot: \n \t Left %f %f %f, \n \t Right %f %f %f"),
		LeftLocationPIDController.P, LeftLocationPIDController.I, LeftLocationPIDController.D,
		RightLocationPIDController.P, RightLocationPIDController.I, RightLocationPIDController.D,
		LeftRotationPIDController.P, LeftRotationPIDController.I, LeftRotationPIDController.D,
		RightRotationPIDController.P, RightRotationPIDController.I, RightRotationPIDController.D);

	// Set MC
	if (InLeftMC && InRightMC)
	{
		SetMCComponents(InLeftMC, InRightMC);
	}
	else
	{
		return false;
	}

	// Set the hands skeletal components
	if (InLeftSkelMeshComp && InRightSkelMeshComp)
	{
		SetHandSkelComponents(InLeftSkelMeshComp, InRightSkelMeshComp);
	}
	else
	{
		return false;
	}
	
	// Set the update functions
	if (LocationControlType != EMCLocationControlType::NONE 
		&& RotationControlType != EMCRotationControlType::NONE)
	{
		// Bind location / rotation update function
		BindLocationUpdateFunction();
		BindRotationUpdateFunction();
	}
	else
	{
		return false;
	}

	// Everything set
	return true;
}

// Set MC components
void FORCEINLINE FMCHandMovement::SetMCComponents(UMotionControllerComponent* InLeftMC, UMotionControllerComponent* InRightMC)
{
	LeftMC = InLeftMC;
	RightMC = InRightMC;
}

// Set hand skeletal components
void FORCEINLINE FMCHandMovement::SetHandSkelComponents(USkeletalMeshComponent* InLeftSkelMeshComp, USkeletalMeshComponent* InRightSkelMeshComp)
{
	LeftSkelMeshComp = InLeftSkelMeshComp;
	RightSkelMeshComp = InRightSkelMeshComp;
}

// Set location PIDs
void FORCEINLINE FMCHandMovement::SetLocationPIDs(const float InLocP, const float InLocI, const float InLocD)
{
	LeftLocationPIDController.P = InLocP;
	LeftLocationPIDController.I = InLocI;
	LeftLocationPIDController.D = InLocD;
	LeftLocationPIDController.Init();

	RightLocationPIDController.P = InLocP;
	RightLocationPIDController.I = InLocI;
	RightLocationPIDController.D = InLocD;
	RightLocationPIDController.Init();
}

// Set rotation PIDs
void FORCEINLINE FMCHandMovement::SetRotationPIDs(const float InRotP, const float InRotI, const float InRotD)
{
	LeftRotationPIDController.P = InRotP;
	LeftRotationPIDController.I = InRotI;
	LeftRotationPIDController.D = InRotD;
	LeftRotationPIDController.Init();

	RightRotationPIDController.P = InRotP;
	RightRotationPIDController.I = InRotI;
	RightRotationPIDController.D = InRotD;
	RightRotationPIDController.Init();
}

// Location control, acceleration based
void FORCEINLINE FMCHandMovement::LocationUpdate_AccBased(float InDeltaTime)
{
	// Calculate location outputs
	const FVector LeftCurrError = LeftMC->GetComponentLocation() - LeftSkelMeshComp->GetComponentLocation();
	const FVector LeftLocOutput = LeftLocationPIDController.Update(LeftCurrError, InDeltaTime);
	const FVector RightCurrError = RightMC->GetComponentLocation() - RightSkelMeshComp->GetComponentLocation();
	const FVector RightLocOutput = RightLocationPIDController.Update(RightCurrError, InDeltaTime);

	// Apply outputs
	LeftSkelMeshComp->AddForceToAllBodiesBelow(LeftLocOutput, NAME_None, true, true);
	RightSkelMeshComp->AddForceToAllBodiesBelow(RightLocOutput, NAME_None, true, true);
}

// Location control, velocity based
void FORCEINLINE FMCHandMovement::LocationUpdate_VelBased(float InDeltaTime)
{
	// Calculate location outputs
	const FVector LeftCurrError = LeftMC->GetComponentLocation() - LeftSkelMeshComp->GetComponentLocation();
	const FVector LeftLocOutput = LeftLocationPIDController.Update(LeftCurrError, InDeltaTime);
	const FVector RightCurrError = RightMC->GetComponentLocation() - RightSkelMeshComp->GetComponentLocation();
	const FVector RightLocOutput = RightLocationPIDController.Update(RightCurrError, InDeltaTime);

	// Apply outputs
	LeftSkelMeshComp->SetAllPhysicsLinearVelocity(LeftLocOutput);
	RightSkelMeshComp->SetAllPhysicsLinearVelocity(RightLocOutput);
}

// Location update
void FORCEINLINE FMCHandMovement::LocationUpdate(float InDeltaTime)
{
	return (this->*LocationUpdateFunctionPtr)(InDeltaTime);
}

// Rotation update
void FORCEINLINE FMCHandMovement::RotationUpdate(float InDeltaTime)
{
	return (this->*RotationUpdateFunctionPtr)(InDeltaTime);
}

// Bind location / rotation update function
void FORCEINLINE FMCHandMovement::BindLocationUpdateFunction()
{
	// Set location control type
	if (LocationControlType == EMCLocationControlType::Acceleration)
	{
		LocationUpdateFunctionPtr = &FMCHandMovement::LocationUpdate_AccBased;
	}
	else if (LocationControlType == EMCLocationControlType::Velocity)
	{
		LocationUpdateFunctionPtr = &FMCHandMovement::LocationUpdate_VelBased;
	}
}

void FORCEINLINE FMCHandMovement::BindRotationUpdateFunction()
{
	// Set location control type
	if (RotationControlType == EMCRotationControlType::Acceleration)
	{
		RotationUpdateFunctionPtr = &FMCHandMovement::LocationUpdate_AccBased;
	}
	else if (RotationControlType == EMCRotationControlType::Velocity)
	{
		RotationUpdateFunctionPtr = &FMCHandMovement::LocationUpdate_VelBased;
	}
}