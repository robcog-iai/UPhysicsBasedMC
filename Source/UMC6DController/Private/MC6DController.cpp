// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Default constructor
FMC6DController::FMC6DController()
{
	bOverwriteTargetLocation = false;
	bApplyToAllChildBodies = false;
	LocUpdateFunctionPointer = &FMC6DController::Loc_Update_NONE;
	RotUpdateFunctionPointer = &FMC6DController::Rot_Update_NONE;
}

// Init as skeletal mesh
void FMC6DController::Init(USceneComponent* InTarget,
	USkeletalMeshComponent* InSelfAsSkeletalMesh,
	bool bApplyToAllBodies,
	EMC6DControlType LocControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	EMC6DControlType RotControlType,
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
	switch (LocControlType)
	{
	case EMC6DControlType::Position:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Position;
		break;
	case EMC6DControlType::Velocity:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Velocity;
		break;
	case EMC6DControlType::Acceleration:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Acceleration;
		break;
	case EMC6DControlType::Force:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Force;
		break;
	case EMC6DControlType::Impulse:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Impulse;
		break;
	default:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_NONE;
		break;
	}

	// Bind update function depending on the control type
	switch (RotControlType)
	{
	case EMC6DControlType::Position:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Position;
		break;
	case EMC6DControlType::Velocity:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Velocity;
		break;
	case EMC6DControlType::Acceleration:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Acceleration;
		break;
	case EMC6DControlType::Force:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Force;
		break;
	case EMC6DControlType::Impulse:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Impulse;
		break;
	default:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_NONE;
		break;
	}
}

// Init as skeletal mesh with an offset
void FMC6DController::Init(USceneComponent* InTarget,
	USkeletalMeshComponent* InSelfAsSkeletalMesh,
	bool bApplyToAllBodies,
	EMC6DControlType LocControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	EMC6DControlType RotControlType,
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
	switch (LocControlType)
	{
	case EMC6DControlType::Position:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Position_Offset;
		break;
	case EMC6DControlType::Velocity:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Velocity_Offset;
		break;
	case EMC6DControlType::Acceleration:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Acceleration_Offset;
		break;
	case EMC6DControlType::Force:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Force_Offset;
		break;
	case EMC6DControlType::Impulse:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Skel_Impulse_Offset;
		break;
	default:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_NONE;
		break;
	}

	switch (RotControlType)
	{
	case EMC6DControlType::Position:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Position_Offset;
		break;
	case EMC6DControlType::Velocity:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Velocity_Offset;
		break;
	case EMC6DControlType::Acceleration:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Acceleration_Offset;
		break;
	case EMC6DControlType::Force:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Force_Offset;
		break;
	case EMC6DControlType::Impulse:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Skel_Impulse_Offset;
		break;
	default:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_NONE;
		break;
	}
}

// Init as static mesh
void FMC6DController::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMC6DControlType LocControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	EMC6DControlType RotControlType,
	float PRot, float IRot, float DRot, float MaxRot)
{
	// Set target and self
	TargetSceneComp = InTarget;
	SelfAsStaticMeshComp = InSelfAsStaticMesh;

	// Init pid controllers
	PIDLoc.Init(PLoc, ILoc, DLoc, MaxLoc);
	PIDRot.Init(PRot, IRot, DRot, MaxRot);

	// Bind update function depending on the control type
	switch (LocControlType)
	{
	case EMC6DControlType::Position:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Position;
		break;
	case EMC6DControlType::Velocity:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Velocity;
		break;
	case EMC6DControlType::Acceleration:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Acceleration;
		break;
	case EMC6DControlType::Force:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Force;
		break;
	case EMC6DControlType::Impulse:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Impulse;
		break;
	default:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_NONE;
		break;
	}

	switch (RotControlType)
	{
	case EMC6DControlType::Position:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Position;
		break;
	case EMC6DControlType::Velocity:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Velocity;
		break;
	case EMC6DControlType::Acceleration:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Acceleration;
		break;
	case EMC6DControlType::Force:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Force;
		break;
	case EMC6DControlType::Impulse:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Impulse;
		break;
	default:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_NONE;
		break;
	}
}

// Init as static mesh with an offset
void FMC6DController::Init(USceneComponent* InTarget,
	UStaticMeshComponent* InSelfAsStaticMesh,
	EMC6DControlType LocControlType,
	float PLoc, float ILoc, float DLoc, float MaxLoc,
	EMC6DControlType RotControlType,
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
	case EMC6DControlType::Position:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Position_Offset;
		break;
	case EMC6DControlType::Velocity:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Velocity_Offset;
		break;
	case EMC6DControlType::Acceleration:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Acceleration_Offset;
		break;
	case EMC6DControlType::Force:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Force_Offset;
		break;
	case EMC6DControlType::Impulse:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_Static_Impulse_Offset;
		break;
	default:
		LocUpdateFunctionPointer = &FMC6DController::Loc_Update_NONE;
		break;
	}

	switch (RotControlType)
	{
	case EMC6DControlType::Position:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Position_Offset;
		break;
	case EMC6DControlType::Velocity:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Velocity_Offset;
		break;
	case EMC6DControlType::Acceleration:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Acceleration_Offset;
		break;
	case EMC6DControlType::Force:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Force_Offset;
		break;
	case EMC6DControlType::Impulse:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_Static_Impulse_Offset;
		break;
	default:
		RotUpdateFunctionPointer = &FMC6DController::Rot_Update_NONE;
		break;
	}
}

// Clear the update function
void FMC6DController::Clear()
{
	LocUpdateFunctionPointer = &FMC6DController::Loc_Update_NONE;
}

// Overwrite the update function to use bone location as target
void FMC6DController::OverwriteToUseBoneForTargetLocation(USkeletalMeshComponent* TargetSkeletalMeshComponent, FName TargetBoneName)
{
	//if (LocUpdateFunctionPointer != NULL)
	//{		
		if (TargetSkeletalMeshComponent && TargetSkeletalMeshComponent->GetBoneIndex(TargetBoneName) != INDEX_NONE)
		{
			OverwriteTargetSkMC = TargetSkeletalMeshComponent;
			OverwriteTargetBoneName = TargetBoneName;
			OverwriteTargetBoneIndex = TargetSkeletalMeshComponent->GetBoneIndex(TargetBoneName);
			bOverwriteTargetLocation = true;
		}
	//}
}

// Reset the location pid controller
void FMC6DController::ResetLoc(float P, float I, float D, float Max, bool bClearErrors /* = true*/)
{
	PIDLoc.Init(P, I, D, Max, bClearErrors);
}

// Call the update function pointer
void FMC6DController::ResetRot(float P, float I, float D, float Max, bool bClearErrors /* = true*/)
{
	PIDRot.Init(P, I, D, Max, bClearErrors);
}

// Call the update function pointer
void FMC6DController::UpdateController(float DeltaTime)
{
	(this->*LocUpdateFunctionPointer)(DeltaTime);
	(this->*RotUpdateFunctionPointer)(DeltaTime);
}

#if UMC_WITH_CHART
// Get the debug chart data
void FMC6DController::GetDebugChartData(FVector& OutLocErr, FVector& OutLocPID, FVector& OutRotErr, FVector& OutRotPID)
{
	OutLocErr = LocErr;
	OutLocPID = LocPID;
	OutRotErr = RotErr;
	OutRotPID = RotPID;
}

// Set the location chart data
void FMC6DController::SetLocDebugChartData(const FVector& InLocErr, const FVector& InLocPID)
{
	LocErr = InLocErr;
	LocPID = InLocPID;
}

// Set the rotation chart data
void FMC6DController::SetRotDebugChartData(const FVector& InRotErr, const FVector& InRotPID)
{
	RotErr = InRotErr;
	RotPID = InRotPID;
}
#endif // UMC_WITH_CHART

// Get the location delta (error)
FORCEINLINE FVector FMC6DController::GetRotationDelta(const FQuat& From, const FQuat& To)
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
void FMC6DController::Loc_Update_NONE(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), *FString(__func__), __LINE__);
}

void FMC6DController::Rot_Update_NONE(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), *FString(__func__), __LINE__);
}


/* Skeletal updates */
// Loc
void FMC6DController::Loc_Update_Skel_Position(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation 
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName) 
		: TargetSceneComp->GetComponentLocation();
	SelfAsSkeletalMeshComp->SetWorldLocation(TargetLoc, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Loc_Update_Skel_Velocity(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->SetAllPhysicsLinearVelocity(OutLoc);
	}
	else
	{
		SelfAsSkeletalMeshComp->SetPhysicsLinearVelocity(OutLoc);
	}

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Skel_Impulse(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsSkeletalMeshComp->AddImpulse(OutLoc);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->AddImpulseToAllBodiesBelow(OutLoc);
	}
	else
	{
		SelfAsSkeletalMeshComp->AddImpulse(OutLoc);
	}

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Skel_Acceleration(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->AddForceToAllBodiesBelow(OutLoc, NAME_None, true);
	}
	else
	{
		SelfAsSkeletalMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)
	}

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Skel_Force(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->AddForceToAllBodiesBelow(OutLoc);
	}
	else
	{
		SelfAsSkeletalMeshComp->AddForce(OutLoc);
	}

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

// Rot
void FMC6DController::Rot_Update_Skel_Position(float DeltaTime)
{
	SelfAsSkeletalMeshComp->SetWorldRotation(TargetSceneComp->GetComponentQuat(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Rot_Update_Skel_Velocity(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Skel_Impulse(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddAngularImpulseInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Skel_Acceleration(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Skel_Force(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

/* Skeletal updates with offset */
// Loc
void FMC6DController::Loc_Update_Skel_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;

	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	SelfAsSkeletalMeshComp->SetWorldLocation(CurrentTargetOffset.GetLocation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Loc_Update_Skel_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->SetAllPhysicsLinearVelocity(OutLoc);
	}
	else
	{
		SelfAsSkeletalMeshComp->SetPhysicsLinearVelocity(OutLoc);
	}

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Skel_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->AddImpulseToAllBodiesBelow(OutLoc);
	}
	else
	{
		SelfAsSkeletalMeshComp->AddImpulse(OutLoc);
	}


#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Skel_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->AddForceToAllBodiesBelow(OutLoc, NAME_None, true);
	}
	else
	{
		SelfAsSkeletalMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)
	}
#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Skel_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsSkeletalMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	if (bApplyToAllChildBodies)
	{
		SelfAsSkeletalMeshComp->AddForceToAllBodiesBelow(OutLoc);
	}
	else
	{
		SelfAsSkeletalMeshComp->AddForce(OutLoc);
	}

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

// Rot
void FMC6DController::Rot_Update_Skel_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	SelfAsSkeletalMeshComp->SetWorldRotation(CurrentTargetOffset.GetRotation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Rot_Update_Skel_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsSkeletalMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Skel_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsSkeletalMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddAngularImpulseInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Skel_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsSkeletalMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Skel_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsSkeletalMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsSkeletalMeshComp->AddTorqueInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}


/* Static mesh updates */
// Loc
void FMC6DController::Loc_Update_Static_Position(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	SelfAsStaticMeshComp->SetWorldLocation(TargetLoc, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Loc_Update_Static_Velocity(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsLinearVelocity(OutLoc);
	
#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Static_Impulse(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddImpulse(OutLoc);

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Static_Acceleration(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Static_Force(float DeltaTime)
{
	const FVector TargetLoc = bOverwriteTargetLocation
		? OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName)
		: TargetSceneComp->GetComponentLocation();
	const FVector DeltaLoc = TargetLoc - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc);

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

// Rot
void FMC6DController::Rot_Update_Static_Position(float DeltaTime)
{
	SelfAsStaticMeshComp->SetWorldRotation(TargetSceneComp->GetComponentQuat(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Rot_Update_Static_Velocity(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Static_Impulse(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddAngularImpulseInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Static_Acceleration(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Static_Force(float DeltaTime)
{
	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), TargetSceneComp->GetComponentQuat());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}


/* Static mesh updates with offset */
// Loc
void FMC6DController::Loc_Update_Static_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	
	if (bOverwriteTargetLocation)
	{		
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	/* Location and Rotation */
	SelfAsStaticMeshComp->SetWorldLocation(CurrentTargetOffset.GetLocation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Loc_Update_Static_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsLinearVelocity(OutLoc);

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Static_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddImpulse(OutLoc);

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Static_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc, NAME_None, true); // Acceleration based (mass will have no effect)

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

void FMC6DController::Loc_Update_Static_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	if (bOverwriteTargetLocation)
	{
		FTransform BoneTransform(FQuat::Identity, OverwriteTargetSkMC->GetBoneLocation(OverwriteTargetBoneName));
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &BoneTransform);
	}
	else
	{
		FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());
	}

	const FVector DeltaLoc = CurrentTargetOffset.GetLocation() - SelfAsStaticMeshComp->GetComponentLocation();
	const FVector OutLoc = PIDLoc.Update(DeltaLoc, DeltaTime);
	SelfAsStaticMeshComp->AddForce(OutLoc); 

#if UMC_WITH_CHART
	SetLocDebugChartData(DeltaLoc, OutLoc);
#endif // UMC_WITH_CHART
}

// Rot
void FMC6DController::Rot_Update_Static_Position_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	/* Location and Rotation */
	SelfAsStaticMeshComp->SetWorldRotation(CurrentTargetOffset.GetRotation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void FMC6DController::Rot_Update_Static_Velocity_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->SetPhysicsAngularVelocityInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Static_Impulse_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddAngularImpulseInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Static_Acceleration_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot, NAME_None, true); // Acceleration based (mass will have no effect)

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}

void FMC6DController::Rot_Update_Static_Force_Offset(float DeltaTime)
{
	/* Offset target calculation */
	FTransform CurrentTargetOffset;
	FTransform::Multiply(&CurrentTargetOffset, &LocalTargetOffset, &TargetSceneComp->GetComponentTransform());

	const FVector DeltaRotAsVector = GetRotationDelta(SelfAsStaticMeshComp->GetComponentQuat(), CurrentTargetOffset.GetRotation());
	const FVector OutRot = PIDRot.Update(DeltaRotAsVector, DeltaTime);
	SelfAsStaticMeshComp->AddTorqueInRadians(OutRot);

#if UMC_WITH_CHART
	SetRotDebugChartData(DeltaRotAsVector, OutRot);
#endif // UMC_WITH_CHART
}
