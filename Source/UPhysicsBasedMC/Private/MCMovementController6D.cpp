// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCMovementController6D.h"

// Default values of controller
UMCMovementController6D::UMCMovementController6D()
{
	/* Control parameters */
	// Movement controller location PID default parameters
	LocationPIDController.P = 300.0;
	LocationPIDController.I = 0.0f;
	LocationPIDController.D = 55.0f;
	LocationPIDController.MaxOutAbs = 9500.f;

	// Movement controller rotation PID default parameters
	RotationPIDController.P = 128.f;
	RotationPIDController.I = 0.0f;
	RotationPIDController.D = 0.0f;
	RotationPIDController.MaxOutAbs = 1500.f;

	// Use default location for tracking by default
	bUseTrackingOffset = false;
	//bUseCustomBoneForTracking = false;
	
	//// Create tracking offset component
	//TrackingOffsetComp = CreateDefaultSubobject<USceneComponent>(TEXT("TrackingOffset"));
	//TrackingOffsetComp->SetupAttachment();


	// Default control type
	LocationControlType = EMCLocationControlType::Acceleration;
	RotationControlType = EMCRotationControlType::Velocity;

	// Default hand rotation offset
	HandRotationAlignmentOffset = FQuat::Identity;

	// Default control update function ptr
	LocationControlFuncPtr = &UMCMovementController6D::LocationControl_None;
	RotationControlFuncPtr = &UMCMovementController6D::RotationControl_None;
}

// Init hand with the motion controllers
void UMCMovementController6D::Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC)
{
	// Set the hand skeletal mesh
	HandSkelComp = InHand;

	// Set rotation offset of the skeletal mesh to the initial rotation
	if (bUseTrackingOffset)
	{
		HandRotationAlignmentOffset = GetComponentQuat();
	}
	else
	{
		HandRotationAlignmentOffset = HandSkelComp->GetComponentQuat();
	}


	//// Check if custom bone should and can be used for tracking
	//if (bUseCustomBoneForTracking)
	//{
	//	if (HandSkelComp->GetBoneIndex(CustomBoneFName) == INDEX_NONE)
	//	{
	//		bUseCustomBoneForTracking = false;
	//	}
	//	else
	//	{
	//		// Set rotation offset of the skeletal mesh to the initial rotation
	//		//HandRotationAlignmentOffset = HandSkelComp->GetBoneQuaternion(CustomBoneFName);
	//	}
	//}

	// Set the motion controller pointer
	MC = InMC;

	// Init PID controllers
	LocationPIDController.Init();
	RotationPIDController.Init();


	// Set location movement control type (bind to the corresponding function ptr)
	switch (LocationControlType)
	{
	case EMCLocationControlType::NONE:
		LocationControlFuncPtr = &UMCMovementController6D::LocationControl_None;
		break;
	case EMCLocationControlType::Force:
		LocationControlFuncPtr = &UMCMovementController6D::LocationControl_ForceBased;
		break;
	case EMCLocationControlType::Acceleration:
		LocationControlFuncPtr = bUseTrackingOffset ?
			&UMCMovementController6D::LocationControl_AccelBased_Offset : 
			&UMCMovementController6D::LocationControl_AccelBased;
		//LocationControlFuncPtr = &UMCMovementController6D::LocationControl_AccelBased;
		break;
	case EMCLocationControlType::Impulse:
		LocationControlFuncPtr = &UMCMovementController6D::LocationControl_ImpulseBased;
		break;
	case EMCLocationControlType::Velocity:
		LocationControlFuncPtr = &UMCMovementController6D::LocationControl_VelBased;
		break;
	case EMCLocationControlType::Position:
		LocationControlFuncPtr = &UMCMovementController6D::LocationControl_PosBased;
		break;
	default:
		LocationControlFuncPtr = &UMCMovementController6D::LocationControl_None;
		break;
	}

	// Set rotation movement control type (bind to the corresponding function ptr)
	switch (RotationControlType)
	{
	case EMCRotationControlType::NONE:
		RotationControlFuncPtr = &UMCMovementController6D::RotationControl_None;
		break;
	case EMCRotationControlType::Torque:
		RotationControlFuncPtr = &UMCMovementController6D::RotationControl_TorqueBased;
		break;
	case EMCRotationControlType::Acceleration:
		RotationControlFuncPtr = &UMCMovementController6D::RotationControl_AccelBased;
		break;
	case EMCRotationControlType::Impulse:
		RotationControlFuncPtr = &UMCMovementController6D::RotationControl_ImpulseBased;
		break;
	case EMCRotationControlType::Velocity:
		RotationControlFuncPtr = bUseTrackingOffset ?
			&UMCMovementController6D::RotationControl_VelBased/*_Offset */: // Offset does not seem to be relevant for rotation
			&UMCMovementController6D::RotationControl_VelBased;
		//RotationControlFuncPtr = &UMCMovementController6D::RotationControl_VelBased;
		break;
	case EMCRotationControlType::Position:
		RotationControlFuncPtr = &UMCMovementController6D::RotationControl_PosBased;
		break;
	default:
		RotationControlFuncPtr = &UMCMovementController6D::RotationControl_None;
		break;
	}
}

// Update the movement
void UMCMovementController6D::Update(const float DeltaTime)
{
	// Call the movement control functions
	(this->*LocationControlFuncPtr)(DeltaTime);
	(this->*RotationControlFuncPtr)(DeltaTime);
}

// Location interaction functions types
void UMCMovementController6D::LocationControl_None(float InDeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Location control OFF (None)"));
}

void UMCMovementController6D::LocationControl_ForceBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - HandSkelComp->GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	HandSkelComp->AddForce(PIDOut);

	//AddForceToAllBodiesBelow(PIDOut);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCMovementController6D::LocationControl_ImpulseBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - HandSkelComp->GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	HandSkelComp->AddImpulse(PIDOut, NAME_None, true); // mass will have no effect

	//AddImpulse(PIDOut);
	//AddImpulseToAllBodiesBelow(PIDOut, NAME_None, true); // mass will have no effect
	//AddImpulseToAllBodiesBelow(PIDOut);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCMovementController6D::LocationControl_AccelBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - HandSkelComp->GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	HandSkelComp->AddForce(PIDOut, NAME_None, true); // Acceleration based (mass will have no effect)

	//AddForceToAllBodiesBelow(PIDOut, NAME_None, true); // Mass will have no effect
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCMovementController6D::LocationControl_AccelBased_Offset(float InDeltaTime)
{
	//const FVector LocErr = MC->GetComponentLocation() - HandSkelComp->GetBoneLocation(CustomBoneFName);
	const FVector LocErr = MC->GetComponentLocation() - GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	HandSkelComp->AddForce(PIDOut, NAME_None, true); // Acceleration based (mass will have no effect)	

	//AddForceToAllBodiesBelow(PIDOut, NAME_None, true); // Mass will have no effect
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCMovementController6D::LocationControl_VelBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - HandSkelComp->GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	HandSkelComp->SetPhysicsLinearVelocity(PIDOut);

	//SetAllPhysicsLinearVelocity(PIDOut);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] MCLoc=%s, Loc=%s, PIDOut=%s, CompVel=%s"),
	//	*FString(__FUNCTION__),
	//	*MC->GetComponentLocation().ToString(),
	//	*GetComponentLocation().ToString(),
	//	*PIDOut.ToString(),
	//	*ComponentVelocity.ToString());
}

void UMCMovementController6D::LocationControl_PosBased(float InDeltaTime)
{
	// TeleportPhysics flag has to be set for physics based teleportation
	HandSkelComp->SetWorldLocation(MC->GetComponentLocation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	//SetAllPhysicsPosition(MC->GetComponentLocation());
}

// Rotation interaction functions types
void UMCMovementController6D::RotationControl_None(float InDeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Rotation control OFF (None)"));
}

void UMCMovementController6D::RotationControl_TorqueBased(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;
	FQuat CompQuat = HandSkelComp->GetComponentQuat();

	// Check if cos theta from the dot product is negative,
	// avoids taking the long path around the sphere
	const float CosTheta = TargetQuat | CompQuat;
	if (CosTheta < 0)
	{
		CompQuat *= -1.f;
	}

	// Use XYZ from the Quaternion as output
	const FQuat QuatOut = TargetQuat * CompQuat.Inverse();
	const FVector RotOut = FVector(QuatOut.X, QuatOut.Y, QuatOut.Z)
		* RotationPIDController.P; // PID P is used as gain

	HandSkelComp->AddTorqueInRadians(RotOut);

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//AddTorqueInRadians(RotOut); 
}

void UMCMovementController6D::RotationControl_AccelBased(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;
	FQuat CompQuat = HandSkelComp->GetComponentQuat();

	// Check if cos theta from the dot product is negative,
	// avoids taking the long path around the sphere
	const float CosTheta = TargetQuat | CompQuat;
	if (CosTheta < 0)
	{
		CompQuat *= -1.f;
	}

	// Use XYZ from the Quaternion as output
	const FQuat QuatOut = TargetQuat * CompQuat.Inverse();
	const FVector RotOut = FVector(QuatOut.X, QuatOut.Y, QuatOut.Z)
		* RotationPIDController.P; // PID P is used as gain

	HandSkelComp->AddTorqueInRadians(RotOut, NAME_None, true); // Acceleration based (mass will have no effect) 

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//AddTorqueInRadians(RotOut, NAME_None, true)); // Acceleration based (mass will have no effect) 
}

void UMCMovementController6D::RotationControl_ImpulseBased(float InDeltaTime)
{
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Rotation Control AccelBased"));
}

void UMCMovementController6D::RotationControl_VelBased(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;	
	FQuat CompQuat = HandSkelComp->GetComponentQuat();

	// Check if cos theta from the dot product is negative,
	// avoids taking the long path around the sphere
	const float CosTheta = TargetQuat | CompQuat;
	if (CosTheta < 0)
	{
		CompQuat *= -1.f;
	}

	// Use XYZ from the Quaternion as output
	const FQuat QuatOut = TargetQuat * CompQuat.Inverse();
	const FVector RotOut = FVector(QuatOut.X, QuatOut.Y, QuatOut.Z)
		* RotationPIDController.P; // PID P is used as gain

	HandSkelComp->SetPhysicsAngularVelocityInRadians(RotOut);
	//SetAllPhysicsAngularVelocityInRadians(RotOut);

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//SetPhysicsAngularVelocityInRadians(PIDOut);
	////SetAllPhysicsAngularVelocityInRadians(PIDOut);	
}

void UMCMovementController6D::RotationControl_VelBased_Offset(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;
	FQuat CompQuat = GetComponentQuat();
	/*FQuat CompQuat = HandSkelComp->GetBoneQuaternion(CustomBoneFName);*/
	
	// Check if cos theta from the dot product is negative,
	// avoids taking the long path around the sphere
	const float CosTheta = TargetQuat | CompQuat;
	if (CosTheta < 0)
	{
		CompQuat *= -1.f;
	}

	// Use XYZ from the Quaternion as output
	const FQuat QuatOut = TargetQuat * CompQuat.Inverse();
	const FVector RotOut = FVector(QuatOut.X, QuatOut.Y, QuatOut.Z)
		* RotationPIDController.P; // PID P is used as gain

	HandSkelComp->SetPhysicsAngularVelocityInRadians(RotOut);
	//SetAllPhysicsAngularVelocityInRadians(RotOut);

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//SetPhysicsAngularVelocityInRadians(PIDOut);
	////SetAllPhysicsAngularVelocityInRadians(PIDOut);	
}

void UMCMovementController6D::RotationControl_PosBased(float InDeltaTime)
{
	// Teleport flag with physics has to be set since physics is enabled
	HandSkelComp->SetWorldRotation(MC->GetComponentQuat() * HandRotationAlignmentOffset,
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	//SetAllPhysicsRotation(MC->GetComponentQuat() * HandRotationAlignmentOffset);
}


