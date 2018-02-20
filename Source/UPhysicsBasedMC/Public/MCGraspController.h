// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "MCFinger.h"
#include "PhysicsEngine/ConstraintDrives.h"
#include "MCGraspController.generated.h"


/**
* Grasp control type
*/
UENUM()
enum class EGraspStyle : uint8
{
	PowerSphere			UMETA(DisplayName = "PowerSphere")
};

/**
 * Grasp control of the hand
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCGraspController : public UObject
{
	GENERATED_BODY()

public:
	// Constructor, set default values
	UMCGraspController();
	
	// Init grasp controller
	void Init(USkeletalMeshComponent* InHand, EControllerHand InHandType, UInputComponent* InIC = nullptr);

	// Grasp type
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	EGraspStyle GraspStyle;

private:
	// Bind grasping inputs
	void SetupInputBindings(UInputComponent* InIC);

	// Setup fingers // TODO able to modify bones from editor
	void SetupFingers();

	// Get finger constraint
	FConstraintInstance* GetFingerConstraint(const FString& BoneName);

	// Update grasp
	void Update(const float Val);

	// Drive type
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	TEnumAsByte<EAngularDriveMode::Type> AngularDriveMode;

	// Spring value to apply to the angular drive (Position strength)
	UPROPERTY(EditAnywhere, Category = "Grasp Control", meta = (ClampMin = 0))
	float Spring;

	// Damping value to apply to the angular drive (Velocity strength) 
	UPROPERTY(EditAnywhere, Category = "Grasp Control", meta = (ClampMin = 0))
	float Damping;

	// Limit of the force that the angular drive can apply
	UPROPERTY(EditAnywhere, Category = "Grasp Control", meta = (ClampMin = 0))
	float ForceLimit;

	// Drive update multiplier
	UPROPERTY(EditAnywhere, Category = "Grasp Control", meta = (ClampMin = 0))
	float UpdateMultiplier;

	// Skeletal hand to control
	USkeletalMeshComponent* SkeletalHand;

	// Handed
	EControllerHand HandType;

	/* Fingers */
	// Thumb finger 
	FMCFinger Thumb;

	// Index finger 
	FMCFinger Index;

	// Middle finger 
	FMCFinger Middle;

	// Ring finger
	FMCFinger Ring;

	// Pinky finger 
	FMCFinger Pinky;
};
