// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCCharacter.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"

#include "HeadMountedDisplay.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"

// Sets default values
AMCCharacter::AMCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Possess player automatically
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Make the capsule thin, and set it only to collide with static objects (VR Mode)
	GetCapsuleComponent()->SetCapsuleRadius(10);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Spectator"));

	// Set flag default values
	bTargetArrowsVisible = true;
	bUseHandsCurrentRotationAsInitial = true;

	// Crate VRCamera root, set is as child of the root component
	VRCameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRCameraRoot"));
	VRCameraRoot->SetupAttachment(GetRootComponent());
	// Move location to the floor
	VRCameraRoot->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	// Create camera component
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VRCameraRoot);
	
	// Create the motion controller offset (hands in front of the character), attach to root component
	MCRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MCRoot"));
	MCRoot->SetupAttachment(GetRootComponent());
	// Move location to the floor
	MCRoot->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

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
	LeftLocationPIDController.I= 0.0f;
	LeftLocationPIDController.D = 50.0f;
	LeftLocationPIDController.MaxOutAbs = 35000.f;
	RightLocationPIDController.P = 700.0f;
	RightLocationPIDController.I = 0.0f;
	RightLocationPIDController.D = 50.0f;
	RightLocationPIDController.MaxOutAbs = 35000.f;
	// Rotation gain
	RotationGain = 12000.f; //TODO switch to PID, and use as P


	// Default interaction type
	MCLocationInteractionType = EMCLocationInteractionType::Acceleration;
	MCRotationInteractionType = EMCRotationInteractionType::Velocity;

	// Left / Right hand offset
	LeftHandRotationOffset = FQuat::Identity;
	RightHandRotationOffset = FQuat::Identity;
}

// Called when the game starts or when spawned
void AMCCharacter::BeginPlay()
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

	if (LeftHand)
	{
		// Set hand offsets
		if (bUseHandsCurrentRotationAsInitial)
		{
			LeftHandRotationOffset = LeftHand->GetSkeletalMeshComponent()->GetComponentQuat();
		}

		// Teleport hands to the current position of the motion controllers
		LeftHand->SetActorLocationAndRotation(MCLeft->GetComponentLocation(),
			MCLeft->GetComponentQuat() * LeftHandRotationOffset);
	}

	if (RightHand)
	{
		// Set hand offsets
		if (bUseHandsCurrentRotationAsInitial)
		{
			RightHandRotationOffset = RightHand->GetSkeletalMeshComponent()->GetComponentQuat();
		}

		// Teleport hands to the current position of the motion controllers
		RightHand->SetActorLocationAndRotation(MCRight->GetComponentLocation(),
			MCRight->GetComponentQuat() * RightHandRotationOffset);
	}
}

// Called every frame
void AMCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Force based movement of the hands to target location and rotation
	if (LeftHand)
	{
		// Get the skeletal mesh component
		USkeletalMeshComponent* SkelMesh = LeftHand->GetSkeletalMeshComponent();

		//// Calculate location output
		const FVector CurrError = MCLeft->GetComponentLocation() 
			- SkelMesh->GetComponentLocation();
		const FVector LocOutput = LeftLocationPIDController.UpdateAsPD(CurrError, DeltaTime);


		SkelMesh->AddForceToAllBodiesBelow(LocOutput, NAME_None, true, true);
		//// Velocity based control
		//const FVector LocOutput = PIDController.UpdateAsP(Error, DeltaTime);
		//SkelMesh->SetAllPhysicsLinearVelocity(LocOutput);

		//// Rotation
		const FQuat TargetQuat = MCLeft->GetComponentQuat() * LeftHandRotationOffset;
		FQuat CurrQuat = SkelMesh->GetComponentQuat();

		// Dot product to get cos theta
		const float CosTheta = TargetQuat | CurrQuat;
		// Avoid taking the long path around the sphere
		if (CosTheta < 0)
		{
			CurrQuat *= -1.f;
		}
		// Use the xyz part of the Quaternion as the rotation velocity
		const FQuat OutputFromQuat = TargetQuat * CurrQuat.Inverse();
		const FVector RotOutput = FVector(OutputFromQuat.X, OutputFromQuat.Y, OutputFromQuat.Z) * RotationGain;
		SkelMesh->SetAllPhysicsAngularVelocityInRadians(RotOutput);
	}

	if (RightHand)
	{
		// Get the skeletal mesh component
		USkeletalMeshComponent* SkelMesh = RightHand->GetSkeletalMeshComponent();

		//// Calculate location output
		const FVector CurrError = MCRight->GetComponentLocation()
			- SkelMesh->GetComponentLocation();
		const FVector LocOutput = RightLocationPIDController.UpdateAsPD(CurrError, DeltaTime);


		SkelMesh->AddForceToAllBodiesBelow(LocOutput, NAME_None, true, true);
		//// Velocity based control
		//const FVector LocOutput = PIDController.UpdateAsP(Error, DeltaTime);
		//SkelMesh->SetAllPhysicsLinearVelocity(LocOutput);

		//// Rotation
		const FQuat TargetQuat = MCRight->GetComponentQuat() * RightHandRotationOffset;
		FQuat CurrQuat = SkelMesh->GetComponentQuat();

		// Dot product to get cos theta
		const float CosTheta = TargetQuat | CurrQuat;
		// Avoid taking the long path around the sphere
		if (CosTheta < 0)
		{
			CurrQuat *= -1.f;
		}
		// Use the xyz part of the Quaternion as the rotation velocity
		const FQuat OutputFromQuat = TargetQuat * CurrQuat.Inverse();
		const FVector RotOutput = FVector(OutputFromQuat.X, OutputFromQuat.Y, OutputFromQuat.Z) * RotationGain;
		SkelMesh->SetAllPhysicsAngularVelocityInRadians(RotOutput);
	}
}

// Called to bind functionality to input
void AMCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

