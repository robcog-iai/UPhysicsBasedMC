// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DControllerCallbacks.h"

// Default constructor
FMC6DControllerCallbacks::FMC6DControllerCallbacks()
{
	bApplyToAllChildBodies = false;
	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_NONE;
}

// Destructor
FMC6DControllerCallbacks::~FMC6DControllerCallbacks() 
{
}

// Init as skeletal mesh
void FMC6DControllerCallbacks::Init(USceneComponent* InTarget,
	USkeletalMeshComponent* InSelfAsSkeletalMesh,
	bool bApplyToAllBodies,
	EMCControlType ControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	float PRot, float IRot, float DRot, float MaxRot)
{
	// Set target and self
	TargetSceneComp = InTarget;
	SelfAsSkeletalMeshComp = InSelfAsSkeletalMesh;
	bApplyToAllChildBodies = bApplyToAllBodies;

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (ControlType)
	{
	case EMCControlType::Position:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Position;
		break;
	//case EMCControlType::Velocity:
	//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Velocity;
	//	break;
	//case EMCControlType::Acceleration:
	//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Acceleration;
	//	break;
	//case EMCControlType::Force:
	//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Force;
	//	break;
	//case EMCControlType::Impulse:
	//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Impulse;
	//	break;
	default:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_NONE;
		break;
	}
}

// Init as skeletal mesh with an offset
void FMC6DControllerCallbacks::Init(USceneComponent* InTarget,
	USkeletalMeshComponent* InSelfAsSkeletalMesh,
	bool bApplyToAllBodies,
	EMCControlType ControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	float PRot, float IRot, float DRot, float MaxRot,
	FTransform InOffset)
{
	// Set target and self
	TargetSceneComp = InTarget;
	SelfAsSkeletalMeshComp = InSelfAsSkeletalMesh;
	bApplyToAllChildBodies = bApplyToAllBodies;

	// Set target offset
	TargetOffset = InOffset;

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (ControlType)
	{
	case EMCControlType::Position:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Position_Offset;
		break;
		//case EMCControlType::Velocity:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Velocity_Offset;
		//	break;
		//case EMCControlType::Acceleration:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Acceleration_Offset;
		//	break;
		//case EMCControlType::Force:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Force_Offset;
		//	break;
		//case EMCControlType::Impulse:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Impulse_Offset;
		//	break;
	default:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_NONE;
		break;
	}
}

// Init as static mesh
void FMC6DControllerCallbacks::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMCControlType ControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	float PRot, float IRot, float DRot, float MaxRot)
{
	// Set target and self
	TargetSceneComp = InTarget;
	SelfAsStaticMeshComp = InSelfAsStaticMesh;

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (ControlType)
	{
	case EMCControlType::Position:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Position;
		break;
		//case EMCControlType::Velocity:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Velocity;
		//	break;
		//case EMCControlType::Acceleration:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Acceleration;
		//	break;
		//case EMCControlType::Force:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Force;
		//	break;
		//case EMCControlType::Impulse:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Impulse;
		//	break;
	default:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_NONE;
		break;
	}
}

// Init as static mesh with an offset
void FMC6DControllerCallbacks::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMCControlType ControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	float PRot, float IRot, float DRot, float MaxRot,
	FTransform InOffset)
{
	// Set target and self
	TargetSceneComp = InTarget;
	SelfAsStaticMeshComp = InSelfAsStaticMesh;

	// Set target offset
	TargetOffset = InOffset;

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (ControlType)
	{
	case EMCControlType::Position:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Position_Offset;
		break;
		//case EMCControlType::Velocity:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Velocity_Offset;
		//	break;
		//case EMCControlType::Acceleration:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Acceleration_Offset;
		//	break;
		//case EMCControlType::Force:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Force_Offset;
		//	break;
		//case EMCControlType::Impulse:
		//	UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Impulse_Offset;
		//	break;
	default:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_NONE;
		break;
	}
}

// Update
void FMC6DControllerCallbacks::Update(float DeltaTime)
{
	(this->*UpdateFunctionPointer)(DeltaTime);
}

// Default update function
void FMC6DControllerCallbacks::Update_NONE(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Skeletal updates
void FMC6DControllerCallbacks::Update_Skel_Position(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Skeletal updates with offset
void FMC6DControllerCallbacks::Update_Skel_Position_Offset(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Static mesh updates
void FMC6DControllerCallbacks::Update_Static_Position(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Static mesh updates with offset
void FMC6DControllerCallbacks::Update_Static_Position_Offset(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}