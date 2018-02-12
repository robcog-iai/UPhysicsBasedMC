// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "MCFinger.h"
#include "MCHandGrasp.generated.h"


/**
* Grasp control type
*/
UENUM(/*BlueprintType*/)
enum class EGraspType : uint8
{
	PowerSphere			UMETA(DisplayName = "PowerSphere")	
};

/**
 * Grasp control of the hand
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCHandGrasp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Constructor, set default values
	UMCHandGrasp();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Init grasp controller
	void Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC);

	// Grasp type
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	EGraspType GraspType;

private:
	// Bind grasping inputs
	void SetupInputBindings(UMotionControllerComponent* InMC);

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
