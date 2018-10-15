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
	case EMCControlType::Velocity:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Velocity;
		break;
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

	// Calculate target offset
	LocalTargetOffset = SelfAsSkeletalMeshComp->GetComponentTransform().GetRelativeTransform(InOffset);

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (ControlType)
	{
	case EMCControlType::Position:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Position_Offset;
		break;
	case EMCControlType::Velocity:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Skel_Velocity_Offset;
		break;
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
	case EMCControlType::Velocity:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Velocity;
		break;
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

	// Calculate target offset
	LocalTargetOffset = SelfAsStaticMeshComp->GetComponentTransform().GetRelativeTransform(InOffset);

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (ControlType)
	{
	case EMCControlType::Position:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Position_Offset;
		break;
	case EMCControlType::Velocity:
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_Static_Velocity_Offset;
		break;
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

// Call the update function pointer
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
	SelfAsSkeletalMeshComp->SetWorldTransform(TargetSceneComp->GetComponentTransform(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DControllerCallbacks::Update_Skel_Velocity(float DeltaTime)
{
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d Out size=%f"),
		TEXT(__FUNCTION__), __LINE__, OutLoc.Size());
	SelfAsSkeletalMeshComp->SetPhysicsLinearVelocity(OutLoc);
}


// Skeletal updates with offset
void FMC6DControllerCallbacks::Update_Skel_Position_Offset(float DeltaTime)
{
	SelfAsSkeletalMeshComp->SetWorldTransform(TargetSceneComp->GetComponentTransform(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	SelfAsSkeletalMeshComp->AddLocalTransform(LocalTargetOffset,
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DControllerCallbacks::Update_Skel_Velocity_Offset(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}


// Static mesh updates
void FMC6DControllerCallbacks::Update_Static_Position(float DeltaTime)
{
	SelfAsStaticMeshComp->SetWorldTransform(TargetSceneComp->GetComponentTransform(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DControllerCallbacks::Update_Static_Velocity(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d Loc Out size=%f"),
		TEXT(__FUNCTION__), __LINE__, OutLoc.Size());
	SelfAsStaticMeshComp->SetPhysicsLinearVelocity(OutLoc);

	/* Rotation */
	FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat RequiredQuat = ToQuat * FromQuat.Inverse();

	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d \t Required Rot=%s"),
		TEXT(__FUNCTION__), __LINE__, *RequiredQuat.ToString());

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (RequiredQuat.W < 0.f)
	{
		RequiredQuat *= -1.f;
		UE_LOG(LogTemp, Error, TEXT(">> %s::%d \t Required Rot=%s"),
			TEXT(__FUNCTION__), __LINE__, *RequiredQuat.ToString());
	}

	FQuat DeltaRot = FQuat::Identity - RequiredQuat;
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d  \t Delta Rot=%s"),
		TEXT(__FUNCTION__), __LINE__, *DeltaRot.ToString());

	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	FVector DeltaRotAsVector = FVector(DeltaRot.X, DeltaRot.Y, DeltaRot.Z);
	FVector RequiredRotAsVector = FVector(RequiredQuat.X, RequiredQuat.Y, RequiredQuat.Z);

	const FVector OutRot = RequiredRotAsVector * PIDRot.P; // PID P is used as gain
	const FVector OutDeltaRotPID = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	const FVector OutReqRotPID = PIDRot.Update(RequiredRotAsVector, DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d \n \t OutRot=%s \n \t OutDeltaRotPID=%s \n \t OutReqRotPID=%s"),
		TEXT(__FUNCTION__), __LINE__,
		*OutRot.ToString(),
		*OutDeltaRotPID.ToString(),
		*OutReqRotPID.ToString());

	SelfAsStaticMeshComp->SetPhysicsAngularVelocityInRadians(OutReqRotPID);
	UE_LOG(LogTemp, Error, TEXT(">> %s::%d \n \n"), TEXT(__FUNCTION__), __LINE__);
}



// Static mesh updates with offset
void FMC6DControllerCallbacks::Update_Static_Position_Offset(float DeltaTime)
{
	SelfAsStaticMeshComp->SetWorldTransform(TargetSceneComp->GetComponentTransform(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	SelfAsStaticMeshComp->AddLocalTransform(LocalTargetOffset,
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DControllerCallbacks::Update_Static_Velocity_Offset(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}