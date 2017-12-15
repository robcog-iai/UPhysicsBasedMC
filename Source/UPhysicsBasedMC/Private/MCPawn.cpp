// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCPawn.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine.h" //Gengine
#include "HeadMountedDisplay.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"

// Sets default values
AMCPawn::AMCPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Possess player automatically
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Set flag default values
	bTargetArrowsVisible = true;
	bUseHandsCurrentRotationAsInitial = true;

	// Crate VRCamera root, set is as child of the root component
	VRCameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRCameraRoot"));
	VRCameraRoot->SetupAttachment(GetRootComponent());
	
	// Create camera component
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VRCameraRoot);

	// Create the motion controller offset (hands in front of the character), attach to root component
	MCRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MCRoot"));
	MCRoot->SetupAttachment(GetRootComponent());
	
	// Create the left/right motion controllers
	MCLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MCLeft"));
	MCLeft->Hand = EControllerHand::Left;
	MCLeft->SetupAttachment(MCRoot);
	MCRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MCRight"));
	MCRight->Hand = EControllerHand::Right;
	MCRight->SetupAttachment(MCRoot);

	// Create left/right visualization arrow of the motion controllers
	MCLeftTargetArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MCLeftTargetArrow"));
	MCLeftTargetArrow->ArrowSize = 0.1;
	MCLeftTargetArrow->SetupAttachment(MCLeft);
	MCRightTargetArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MCRightTargetArrow"));
	MCRightTargetArrow->ArrowSize = 0.1;
	MCRightTargetArrow->SetupAttachment(MCRight);

	/* Control parameters */
	// Location PID default parameters
	LeftLocationPIDController.P = 700.0f;
	LeftLocationPIDController.I = 0.0f;
	LeftLocationPIDController.D = 50.0f;
	LeftLocationPIDController.MaxOutAbs = 35000.f;
	RightLocationPIDController.P = 700.0f;
	RightLocationPIDController.I = 0.0f;
	RightLocationPIDController.D = 50.0f;
	RightLocationPIDController.MaxOutAbs = 35000.f;
	// Rotation gain
	RotationGain = 12000.f; //TODO switch to PID, and use as P

							// Default interaction type
	LocationControlType = EMCLocationControlType::Acceleration;
	RotationControlType = EMCRotationControlType::Velocity;

	// Left / Right hand offset
	LeftHandRotationOffset = FQuat::Identity;
	RightHandRotationOffset = FQuat::Identity;
}

// Called when the game starts or when spawned
void AMCPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set target arrow visibility at runtime
	MCLeftTargetArrow->SetHiddenInGame(!bTargetArrowsVisible);
	MCRightTargetArrow->SetHiddenInGame(!bTargetArrowsVisible);

	// Check if VR is enabled
	IHeadMountedDisplay* HMD = (IHeadMountedDisplay*)(GEngine->XRSystem->GetHMDDevice());
	if (HMD && HMD->IsHMDEnabled())
	{
		GEngine->XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}

	// Set control types and enable tick
	SetActorTickEnabled(CheckHandsAndInitControllers());
}

// Called every frame
void AMCPawn::Tick(float DeltaTime)
{
	(this->*LocationControlFunctionPtr)(DeltaTime);
	(this->*RotationControlFunctionPtr)(DeltaTime);
}

// Called to bind functionality to input
void AMCPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Set location and rotation control type
bool AMCPawn::CheckHandsAndInitControllers()
{
	// Make sure both hands are set
	if (LeftHand && RightHand)
	{
		// Get the skeletal mesh components
		LeftSkeletalMeshComponent = LeftHand->GetSkeletalMeshComponent();
		RightSkeletalMeshComponent = RightHand->GetSkeletalMeshComponent();

		// Continue if both hands have a skeletal mesh component
		if (LeftSkeletalMeshComponent && RightSkeletalMeshComponent)
		{
			// Set hand offsets
			if (bUseHandsCurrentRotationAsInitial)
			{
				LeftHandRotationOffset = LeftHand->GetSkeletalMeshComponent()->GetComponentQuat();
				RightHandRotationOffset = RightHand->GetSkeletalMeshComponent()->GetComponentQuat();
			}

			// Teleport the hands to the MC controller position 
			// (this however is the default pose in begin play, since tracking is not active yet)
			/*LeftHand->SetActorLocationAndRotation(MCLeft->GetComponentLocation(),
			MCLeft->GetComponentQuat() * LeftHandRotationOffset,
			false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			RightHand->SetActorLocationAndRotation(MCRight->GetComponentLocation(),
			MCRight->GetComponentQuat() * RightHandRotationOffset,
			false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);*/

			// Set location control type
			if (LocationControlType == EMCLocationControlType::Acceleration)
			{
				LocationControlFunctionPtr = &AMCPawn::LocationControl_AccBased;
			}
			else if (LocationControlType == EMCLocationControlType::Velocity)
			{
				LocationControlFunctionPtr = &AMCPawn::LocationControl_VelBased;
			}
			else
			{
				return false;
			}

			// Set rotation control type
			if (RotationControlType == EMCRotationControlType::Velocity)
			{
				RotationControlFunctionPtr = &AMCPawn::RotationControl_VelBased;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// Hands, skeletal meshes, and controllers are set, tick can be enabled
	UE_LOG(LogTemp, Warning, TEXT("Character line: %s "), *FString::FromInt(__LINE__));
	return true;
}

// Location control, acceleration based
void AMCPawn::LocationControl_AccBased(float InDeltaTime)
{
	// Calculate location outputs
	const FVector LeftCurrError = MCLeft->GetComponentLocation() - LeftSkeletalMeshComponent->GetComponentLocation();
	const FVector LeftLocOutput = LeftLocationPIDController.UpdateAsPD(LeftCurrError, InDeltaTime);
	const FVector RightCurrError = MCRight->GetComponentLocation() - RightSkeletalMeshComponent->GetComponentLocation();
	const FVector RightLocOutput = RightLocationPIDController.UpdateAsPD(RightCurrError, InDeltaTime);

	// Apply outputs
	LeftSkeletalMeshComponent->AddForceToAllBodiesBelow(LeftLocOutput, NAME_None, true, true);
	RightSkeletalMeshComponent->AddForceToAllBodiesBelow(RightLocOutput, NAME_None, true, true);
}

// Location control, velocity based
void AMCPawn::LocationControl_VelBased(float InDeltaTime)
{
	// Calculate location outputs
	const FVector LeftCurrError = MCLeft->GetComponentLocation() - LeftSkeletalMeshComponent->GetComponentLocation();
	const FVector LeftLocOutput = LeftLocationPIDController.UpdateAsPD(LeftCurrError, InDeltaTime);
	const FVector RightCurrError = MCRight->GetComponentLocation() - RightSkeletalMeshComponent->GetComponentLocation();
	const FVector RightLocOutput = RightLocationPIDController.UpdateAsPD(RightCurrError, InDeltaTime);

	// Apply outputs
	LeftSkeletalMeshComponent->SetAllPhysicsLinearVelocity(LeftLocOutput);
	RightSkeletalMeshComponent->SetAllPhysicsLinearVelocity(RightLocOutput);
}

// Rotation control, velocity based
void AMCPawn::RotationControl_VelBased(float InDeltaTime)
{
	// Get the (offsetted) target and current rotations
	const FQuat LeftTargetQuat = MCLeft->GetComponentQuat() * LeftHandRotationOffset;
	FQuat LeftCurrQuat = LeftSkeletalMeshComponent->GetComponentQuat();
	const FQuat RightTargetQuat = MCRight->GetComponentQuat() * RightHandRotationOffset;
	FQuat RightCurrQuat = RightSkeletalMeshComponent->GetComponentQuat();

	//// Calculate rotation outputs
	// Dot product to get cos theta
	const float LeftCosTheta = LeftTargetQuat | LeftCurrQuat;
	// Avoid taking the long path around the sphere
	if (LeftCosTheta < 0)
	{
		LeftCurrQuat *= -1.f;
	}
	// Use the xyz part of the Quaternion as the rotation velocity
	const FQuat LeftOutputFromQuat = LeftTargetQuat * LeftCurrQuat.Inverse();
	const FVector LeftRotOutput = FVector(LeftOutputFromQuat.X, LeftOutputFromQuat.Y, LeftOutputFromQuat.Z) * RotationGain;

	// Dot product to get cos theta
	const float RightCosTheta = RightTargetQuat | RightCurrQuat;
	// Avoid taking the long path around the sphere
	if (RightCosTheta < 0)
	{
		RightCurrQuat *= -1.f;
	}
	// Use the xyz part of the Quaternion as the rotation velocity
	const FQuat RightOutputFromQuat = RightTargetQuat * RightCurrQuat.Inverse();
	const FVector RightRotOutput = FVector(RightOutputFromQuat.X, RightOutputFromQuat.Y, RightOutputFromQuat.Z) * RotationGain;

	//// Apply outputs	
	LeftSkeletalMeshComponent->SetAllPhysicsAngularVelocityInRadians(LeftRotOutput);
	RightSkeletalMeshComponent->SetAllPhysicsAngularVelocityInRadians(RightRotOutput);
}