// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MCGraspAnimDataAsset.h"
#include "MCGraspAnimController.generated.h"

/**
* Hand type
*/
UENUM()
enum class EMCGraspAnimHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};

/**
* The controller chooses the right animations and sends them to the executor
*/
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Grasp Anim Controller") )
class UMCGRASP_API UMCGraspAnimController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCGraspAnimController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

private:
	// Init the component, return false is something went wrong
	bool Init();

	// Prepare the skeletal mesh component physics and angular motors
	bool LoadSkeletalMesh();

	// Load the data from the animation data assets in a more optimized form, return true if at least one animation is loaded
	bool LoadAnimationData();

	// Set first grasp animation
	void GotoFirstAnimation();

	// Bind user inputs for updating the grasps and switching the animations
	void SetupInputBindings();

	// Set the cached target to the first frame 
	void SetTargetToIdle();

	// Compute and set the cached target by interpolating between the two frames
	void SetTargetUsingLerp(const TMap<FConstraintInstance*, FRotator>& FrameA, const TMap<FConstraintInstance*, FRotator>& FrameB, float Alpha);

	// Set the drive parameters to the cached target
	void DriveToTarget();

	/* Input callbacks */
	// Update the grasp animation from the trigger input
	void GraspUpdateCallback(float Value);

	// Switch to the next grasp animation
	void GotoNextAnimationCallback();

	// Switch to the previous animation
	void GotoPreviousAnimationCallback();


	// Stops the grasping process and resets the booleans that were changed
	void StopGrasping();

	// Loads the current grasp data from the data asset
	void SetActiveGrasp();

private:
#if WITH_EDITOR
	// Hand type, to listen to the right inputs
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	EMCGraspAnimHandType HandType;
#endif // WITH_EDITOR

	// The input to grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputAxisName;

	// The input to select the next grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputNextAnimAction;

	// The input to select the previous grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputPrevAnimAction;

	// An array the user can fill with grasps they want to use
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TArray<UMCGraspAnimDataAsset*> AnimationDataAssets;

	// Idle spring value (used to keep the fingers steady when the trigger is released)
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringIdle;

	// Multiply the spring value relative to the trigger pressed value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringActiveMultiplier;

	// Damping value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float Damping;

	// Force limit, 0 means no limit. 
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float ForceLimit;

	// Skeletal mesh to apply the animation on
	USkeletalMeshComponent* SkelComp;


	typedef TMap<FConstraintInstance*, FRotator> FFrame;
	typedef TArray<FFrame> FAnimation;

	// Drive target (map between the constraint instance reference and the rotation)
	FFrame DriveTarget;

	// Selected animation
	FAnimation ActiveAnimation;
	
	// Animation list
	TArray<FAnimation> Animations;





	// Cache the constraints references directly to the animation rotation data

	// Currently active grasp animation index
	int32 ActiveAnimIndex;

	// Flag showing inf the grasp is ongoing
	bool bGraspIsActive;




	// Spring value during an active grasp
	float SpringActive;

	//// Current grasp loaded into hand
	//UMCGraspAnimDataAsset* ActiveAnimDA;

	//// When changing grasp type while grasping, the new grasp isn't applied immediately
	//// Instead it is saved in this variable and applied once the user stops grasping 
	//UMCGraspAnimDataAsset* QueuedAnimDA;

	// Set to true when there is a grasp waiting to be applied
	bool bGrasIsWaitingInQueue;

	// bool that is used so the mesh goes into step 0 of it's current grasp when the game is started
	bool bFirstUpdate;
};
