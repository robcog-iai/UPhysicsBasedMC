// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DController.h"

// Default constructor
FMC6DController::FMC6DController()
{
	bApplyToAllChildBodies = false;
	UpdateFunctionPointer = &FMC6DController::Update_NONE;
}

// Destructor
FMC6DController::~FMC6DController() 
{
}

// Init as skeletal mesh
void FMC6DController::Init(USceneComponent* InTarget,
	USkeletalMeshComponent* InSelfAsSkeletalMesh,
	bool bApplyToAllBodies,
	EMC6DControlType ControlType,
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
	case EMC6DControlType::Position:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Position;
		break;
	case EMC6DControlType::Velocity:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Velocity;
		break;
	case EMC6DControlType::Acceleration:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Acceleration;
		break;
	case EMC6DControlType::Force:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Force;
		break;
	case EMC6DControlType::Impulse:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Impulse;
		break;
	default:
		UpdateFunctionPointer = &FMC6DController::Update_NONE;
		break;
	}
}

// Init as skeletal mesh with an offset
void FMC6DController::Init(USceneComponent* InTarget,
	USkeletalMeshComponent* InSelfAsSkeletalMesh,
	bool bApplyToAllBodies,
	EMC6DControlType ControlType,
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
	case EMC6DControlType::Position:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Position_Offset;
		break;
	case EMC6DControlType::Velocity:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Velocity_Offset;
		break;
	case EMC6DControlType::Acceleration:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Acceleration_Offset;
		break;
	case EMC6DControlType::Force:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Force_Offset;
		break;
	case EMC6DControlType::Impulse:
		UpdateFunctionPointer = &FMC6DController::Update_Skel_Impulse_Offset;
		break;
	default:
		UpdateFunctionPointer = &FMC6DController::Update_NONE;
		break;
	}
}

// Init as static mesh
void FMC6DController::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMC6DControlType ControlType,
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
	case EMC6DControlType::Position:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Position;
		break;
	case EMC6DControlType::Velocity:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Velocity;
		break;
	case EMC6DControlType::Acceleration:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Acceleration;
		break;
	case EMC6DControlType::Force:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Force;
		break;
	case EMC6DControlType::Impulse:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Impulse;
		break;
	default:
		UpdateFunctionPointer = &FMC6DController::Update_NONE;
		break;
	}
}

// Init as static mesh with an offset
void FMC6DController::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMC6DControlType ControlType,
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
	case EMC6DControlType::Position:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Position_Offset;
		break;
	case EMC6DControlType::Velocity:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Velocity_Offset;
		break;
	case EMC6DControlType::Acceleration:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Acceleration_Offset;
		break;
	case EMC6DControlType::Force:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Force_Offset;
		break;
	case EMC6DControlType::Impulse:
		UpdateFunctionPointer = &FMC6DController::Update_Static_Impulse_Offset;
		break;
	default:
		UpdateFunctionPointer = &FMC6DController::Update_NONE;
		break;
	}
}

// Call the update function pointer
void FMC6DController::Update(float DeltaTime)
{
	(this->*UpdateFunctionPointer)(DeltaTime);
}

// Default update function
void FMC6DController::Update_NONE(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}


// Skeletal updates
void FMC6DController::Update_Skel_Position(float DeltaTime)
{
	SelfAsSkeletalMeshComp->SetWorldTransform(TargetSceneComp->GetComponentTransform(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Update_Skel_Velocity(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->SetPhysicsLinearVelocity(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);
}

void FMC6DController::Update_Skel_Acceleration(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)
}

void FMC6DController::Update_Skel_Force(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddForce(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot);
}

void FMC6DController::Update_Skel_Impulse(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddImpulse(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddAngularImpulseInRadians(OutRot);
}



// Skeletal updates with offset
void FMC6DController::Update_Skel_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location and Rotation */
	SelfAsSkeletalMeshComp->SetWorldTransform(CurrentTargetOffset,
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Update_Skel_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->SetPhysicsLinearVelocity(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsSkeletalMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);
}

void FMC6DController::Update_Skel_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)

	/* Rotation */
	const FQuat FromQuat = SelfAsSkeletalMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)
}

void FMC6DController::Update_Skel_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddForce(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsSkeletalMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot);
}

void FMC6DController::Update_Skel_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddImpulse(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsSkeletalMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddAngularImpulseInRadians(OutRot);
}


// Static mesh updates
void FMC6DController::Update_Static_Position(float DeltaTime)
{
	SelfAsStaticMeshComp->SetWorldTransform(TargetSceneComp->GetComponentTransform(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Update_Static_Velocity(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsLinearVelocity(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);
}

void FMC6DController::Update_Static_Acceleration(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)
}

void FMC6DController::Update_Static_Force(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot);
}

void FMC6DController::Update_Static_Impulse(float DeltaTime)
{
	/* Location */
	const FVector DeltaLoc = TargetSceneComp->GetComponentLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddImpulse(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = TargetSceneComp->GetComponentQuat();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddAngularImpulseInRadians(OutRot);
}


// Static mesh updates with offset
void FMC6DController::Update_Static_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location and Rotation */
	SelfAsStaticMeshComp->SetWorldTransform(CurrentTargetOffset,
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Update_Static_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsLinearVelocity(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);
}

void FMC6DController::Update_Static_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)
}

void FMC6DController::Update_Static_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc); 

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot); 
}

void FMC6DController::Update_Static_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location */
	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddImpulse(OutLoc);

	/* Rotation */
	const FQuat FromQuat = SelfAsStaticMeshComp->GetComponentQuat();
	const FQuat ToQuat = CurrentTargetOffset.GetRotation();
	FQuat DeltaQuat = ToQuat * FromQuat.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	const FVector DeltaRotAsVector = FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddAngularImpulseInRadians(OutRot);
}
