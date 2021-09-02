// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspHelper6DPIDController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Default constructor
FMCGraspHelper6DPIDController::FMCGraspHelper6DPIDController()
{
	LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_NONE;
	RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_NONE;
}

// Init as static mesh with an offset
void FMCGraspHelper6DPIDController::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMCGraspHelp6DControlType LocControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	EMCGraspHelp6DControlType RotControlType,
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
	switch (LocControlType)
	{
	case EMCGraspHelp6DControlType::Position:
		LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_Static_Position_Offset;
		break;
	case EMCGraspHelp6DControlType::Velocity:
		LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_Static_Velocity_Offset;
		break;
	case EMCGraspHelp6DControlType::Acceleration:
		LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_Static_Acceleration_Offset;
		break;
	case EMCGraspHelp6DControlType::Force:
		LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_Static_Force_Offset;
		break;
	case EMCGraspHelp6DControlType::Impulse:
		LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_Static_Impulse_Offset;
		break;
	default:
		LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_NONE;
		break;
	}

	switch (RotControlType)
	{
	case EMCGraspHelp6DControlType::Position:
		RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_Static_Position_Offset;
		break;
	case EMCGraspHelp6DControlType::Velocity:
		RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_Static_Velocity_Offset;
		break;
	case EMCGraspHelp6DControlType::Acceleration:
		RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_Static_Acceleration_Offset;
		break;
	case EMCGraspHelp6DControlType::Force:
		RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_Static_Force_Offset;
		break;
	case EMCGraspHelp6DControlType::Impulse:
		RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_Static_Impulse_Offset;
		break;
	default:
		RotUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Rot_Update_NONE;
		break;
	}
}

// Clear the update function
void FMCGraspHelper6DPIDController::Clear()
{
	LocUpdateFunctionPointer = &FMCGraspHelper6DPIDController::Loc_Update_NONE;
}


// Reset the location pid controller
void FMCGraspHelper6DPIDController::ResetLoc(float P, float I, float D, float Max, bool bClearErrors /* = true*/)
{
	PIDLoc.Init(P, I, D, Max, bClearErrors);
}

// Call the update function pointer
void FMCGraspHelper6DPIDController::ResetRot(float P, float I, float D, float Max, bool bClearErrors /* = true*/)
{
	PIDRot.Init(P, I, D, Max, bClearErrors);
}

// Call the update function pointer
void FMCGraspHelper6DPIDController::UpdateController(float DeltaTime)
{
	(this->*LocUpdateFunctionPointer)(DeltaTime);
	(this->*RotUpdateFunctionPointer)(DeltaTime);
}


// Get the location delta (error)
FORCEINLINE FVector FMCGraspHelper6DPIDController::GetRotationDelta(const FQuat& From, const FQuat& To)
{
	// TODO test internal versions as well using FQuat/FRotator SLerp / Lerp
	// Get the delta between the quaternions
	FQuat DeltaQuat = To * From.Inverse();

	// Avoid taking the long path around the sphere
	// // See void FQuat::EnforceShortestArcWith(const FQuat& OtherQuat)
	//	const float CosTheta = ToQuat | FromQuat;
	//	if (CosTheta < 0)
	if (DeltaQuat.W < 0.f)
	{
		DeltaQuat *= -1.f;
	}
	// The W part of the vector is always ~1.f, not relevant for applying the rotation
	return FVector(DeltaQuat.X, DeltaQuat.Y, DeltaQuat.Z);
}

// Default update function
void FMCGraspHelper6DPIDController::Loc_Update_NONE(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), *FString(__func__), __LINE__);
}

void FMCGraspHelper6DPIDController::Rot_Update_NONE(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), *FString(__func__), __LINE__);
}



/* Static mesh updates with offset */
// Loc
void FMCGraspHelper6DPIDController::Loc_Update_Static_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location and Rotation */
	SelfAsStaticMeshComp->SetWorldLocation(CurrentTargetOffset.GetLocation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMCGraspHelper6DPIDController::Loc_Update_Static_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsLinearVelocity(OutLoc);
}

void FMCGraspHelper6DPIDController::Loc_Update_Static_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddImpulse(OutLoc);
}

void FMCGraspHelper6DPIDController::Loc_Update_Static_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)
}

void FMCGraspHelper6DPIDController::Loc_Update_Static_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc); 
}

// Rot
void FMCGraspHelper6DPIDController::Rot_Update_Static_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location and Rotation */
	SelfAsStaticMeshComp->SetWorldRotation(CurrentTargetOffset.GetRotation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMCGraspHelper6DPIDController::Rot_Update_Static_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);
}

void FMCGraspHelper6DPIDController::Rot_Update_Static_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddAngularImpulseInRadians(OutRot);
}

void FMCGraspHelper6DPIDController::Rot_Update_Static_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)
}

void FMCGraspHelper6DPIDController::Rot_Update_Static_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot);
}
