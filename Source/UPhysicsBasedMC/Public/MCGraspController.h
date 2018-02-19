// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "MCFinger.h"
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
	void Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC, UInputComponent* InIC = nullptr);

	// Grasp type
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	EGraspStyle GraspStyle;

private:
	// Bind grasping inputs
	void SetupInputBindings(UMotionControllerComponent* InMC, UInputComponent* InIC);

	// Update grasp
	void Update(const float Val);

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
