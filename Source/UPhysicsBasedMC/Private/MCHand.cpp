// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCHand.h"
//#include "IXRTrackingSystem.h"
//#include "XRMotionControllerBase.h"


// Sets default values
UMCHand::UMCHand()
{
	// Disable ticking in the editor
	bTickInEditor = false;

	// Default values for the skeletal mesh
	SetSimulatePhysics(true);
	SetEnableGravity(false);
	SetCollisionProfileName(TEXT("BlockAll"));
	bGenerateOverlapEvents = true;
	
	/* Control parameters */
	// Location PID default parameters
	LocationPIDController.P = 250.0;
	LocationPIDController.I = 0.0f;
	LocationPIDController.D = 50.0f;
	LocationPIDController.MaxOutAbs = 1500.f;	

	// Location PID default parameters
	RotationPIDController.P = 100.f;
	RotationPIDController.I = 0.0f;
	RotationPIDController.D = 0.0f;
	RotationPIDController.MaxOutAbs = 1500.f;	

	// Default control type
	LocationControlType = EMCLocationControlType::Acceleration;
	RotationControlType = EMCRotationControlType::Velocity;

	// Default hand rotation offset
	HandRotationAlignmentOffset = FQuat::Identity;
}

// Called when the game starts or when spawned
void UMCHand::BeginPlay()
{
	Super::BeginPlay();
	
	// Default control update function ptr
	LocationControlFuncPtr = &UMCHand::LocationControl_None;
	RotationControlFuncPtr = &UMCHand::RotationControl_None;

	// Init PID controllers
	LocationPIDController.Init();
	RotationPIDController.Init();

	// Set rotation offset of the skeletal mesh to the initial rotation
	HandRotationAlignmentOffset = GetComponentQuat();

	// Disable tick by default, enable it in Init()
	SetComponentTickEnabled(false);
}

// Called every frame, used for motion control
void UMCHand::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Call the movement control functions
	(this->*LocationControlFuncPtr)(DeltaTime);
	(this->*RotationControlFuncPtr)(DeltaTime);
}

// Init hand with the motion controllers
void UMCHand::Init(UMotionControllerComponent* InMC)
{
	// Set the motion controller pointer
	MC = InMC;

	// Set location movement control type (bind to the corresponding function ptr)
	switch (LocationControlType)
	{
	case EMCLocationControlType::NONE:
		LocationControlFuncPtr = &UMCHand::LocationControl_None;
		break;
	case EMCLocationControlType::Force:
		LocationControlFuncPtr = &UMCHand::LocationControl_ForceBased;
		break;
	case EMCLocationControlType::Acceleration:
		LocationControlFuncPtr = &UMCHand::LocationControl_AccelBased;
		break;
	case EMCLocationControlType::Impulse:
		LocationControlFuncPtr = &UMCHand::LocationControl_ImpulseBased;
		break;
	case EMCLocationControlType::Velocity:
		LocationControlFuncPtr = &UMCHand::LocationControl_VelBased;
		break;
	case EMCLocationControlType::Position:
		LocationControlFuncPtr = &UMCHand::LocationControl_PosBased;
		break;
	default:
		LocationControlFuncPtr = &UMCHand::LocationControl_None;
		break;
	}

	// Set rotation movement control type (bind to the corresponding function ptr)
	switch (RotationControlType)
	{
	case EMCRotationControlType::NONE:
		RotationControlFuncPtr = &UMCHand::RotationControl_None;
		break;
	case EMCRotationControlType::Torque:
		RotationControlFuncPtr = &UMCHand::RotationControl_TorqueBased;
		break;
	case EMCRotationControlType::Acceleration:
		RotationControlFuncPtr = &UMCHand::RotationControl_AccelBased;
		break;
	case EMCRotationControlType::Impulse:
		RotationControlFuncPtr = &UMCHand::RotationControl_ImpulseBased;
		break;
	case EMCRotationControlType::Velocity:
		RotationControlFuncPtr = &UMCHand::RotationControl_VelBased;
		break;
	case EMCRotationControlType::Position:
		RotationControlFuncPtr = &UMCHand::RotationControl_PosBased;
		break;
	default:
		RotationControlFuncPtr = &UMCHand::RotationControl_None;
		break;
	}
		
	// Enable Tick
	SetComponentTickEnabled(true);
}

// Location interaction functions types
void UMCHand::LocationControl_None(float InDeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Location control OFF (None)"));
}

void UMCHand::LocationControl_ForceBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	AddForce(PIDOut);

	//AddForceToAllBodiesBelow(PIDOut);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCHand::LocationControl_ImpulseBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	AddImpulse(PIDOut, NAME_None, true); // mass will have no effect

	//AddImpulse(PIDOut);
	//AddImpulseToAllBodiesBelow(PIDOut, NAME_None, true); // mass will have no effect
	//AddImpulseToAllBodiesBelow(PIDOut);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCHand::LocationControl_AccelBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	AddForce(PIDOut, NAME_None, true); // Acceleration based (mass will have no effect)	

	//AddForceToAllBodiesBelow(PIDOut, NAME_None, true); // Mass will have no effect
	//UE_LOG(LogTemp, Warning, TEXT("[%s] PIDOut=%s"),
	//	*FString(__FUNCTION__), *PIDOut.ToString());
}

void UMCHand::LocationControl_VelBased(float InDeltaTime)
{
	const FVector LocErr = MC->GetComponentLocation() - GetComponentLocation();
	const FVector PIDOut = LocationPIDController.Update(LocErr, InDeltaTime);
	SetPhysicsLinearVelocity(PIDOut);

	//SetAllPhysicsLinearVelocity(PIDOut);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] MCLoc=%s, Loc=%s, PIDOut=%s, CompVel=%s"),
	//	*FString(__FUNCTION__),
	//	*MC->GetComponentLocation().ToString(),
	//	*GetComponentLocation().ToString(),
	//	*PIDOut.ToString(),
	//	*ComponentVelocity.ToString());
}

void UMCHand::LocationControl_PosBased(float InDeltaTime)
{	
	// TeleportPhysics flag has to be set for physics based teleportation
	SetWorldLocation(MC->GetComponentLocation(),
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics); 
	//SetAllPhysicsPosition(MC->GetComponentLocation());
}

// Rotation interaction functions types
void UMCHand::RotationControl_None(float InDeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Rotation control OFF (None)"));
}

void UMCHand::RotationControl_TorqueBased(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;
	FQuat CompQuat = GetComponentQuat();

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

	AddTorqueInRadians(RotOut); 

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//AddTorqueInRadians(RotOut); 
}

void UMCHand::RotationControl_AccelBased(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;
	FQuat CompQuat = GetComponentQuat();

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

	AddTorqueInRadians(RotOut, NAME_None, true); // Acceleration based (mass will have no effect) 

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//AddTorqueInRadians(RotOut, NAME_None, true)); // Acceleration based (mass will have no effect) 
}

void UMCHand::RotationControl_ImpulseBased(float InDeltaTime)
{
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Rotation Control AccelBased"));
}

void UMCHand::RotationControl_VelBased(float InDeltaTime)
{
	const FQuat TargetQuat = MC->GetComponentQuat() * HandRotationAlignmentOffset;
	FQuat CompQuat = GetComponentQuat();

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

	SetPhysicsAngularVelocityInRadians(RotOut);
	//SetAllPhysicsAngularVelocityInRadians(RotOut);

	// PID Version
	//const FRotator RotErr = MC->GetComponentRotation() - GetComponentRotation();
	//const FVector PIDOut = RotationPIDController.Update(RotErr.Vector(), InDeltaTime);
	//SetPhysicsAngularVelocityInRadians(PIDOut);
	////SetAllPhysicsAngularVelocityInRadians(PIDOut);	
}

void UMCHand::RotationControl_PosBased(float InDeltaTime)
{
	// Teleport flag with physics has to be set since physics is enabled
	SetWorldRotation(MC->GetComponentQuat() * HandRotationAlignmentOffset,
		false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	//SetAllPhysicsRotation(MC->GetComponentQuat() * HandRotationAlignmentOffset);
}