// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

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

/** Notify the active grasp type */
DECLARE_MULTICAST_DELEGATE_OneParam(FMCGraspTypeSignature, const FString& /*GraspType*/);

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
	// Frame and array of frames (animation)
	typedef TMap<FConstraintInstance*, FRotator> FFrame;
	typedef TArray<FFrame> FAnimation;

	// Init the component
	void Init();

	// Prepare the skeletal mesh component physics and angular motors
	bool LoadSkeletalMesh();

	// Load the data from the animation data assets in a more optimized form, return true if at least one animation is loaded
	bool LoadAnimationData();

	// Set the motors target value to the first frame
	void DriveToFirstFrame();

	// Set the motors target value to the final frame
	void DriveToLastFrame();

	// Bind user inputs for updating the grasps and switching the animations
	void SetupInputBindings();

	// Compute and set the cached target by interpolating between the two frames
	void SetTargetUsingLerp(const FFrame& FrameA, const FFrame& FrameB, float Alpha);

	// Set the drive parameters to the cached target
	void DriveToTarget();

	// Calculate the active frame relative to the input value (0 - 1)
	int32 GetActiveFrameIndex(float Value);

	/* Input callbacks */
	// Update the grasp animation from the trigger input
	void GraspUpdateCallback(float Value);

	// Switch to the next grasp animation
	void GotoNextAnimationCallback();

	// Switch to the previous animation
	void GotoPreviousAnimationCallback();

public:
	// Publishes the current active grasp type
	FMCGraspTypeSignature OnGraspType;
	
private:
	// Skip initialization if true
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	uint8 bIgnore : 1;

	// Skip initialization if true
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	uint8 bLogDebug : 1;

#if WITH_EDITORONLY_DATA
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

	// Idle spring value (used to keep the fingers steady when the trigger is released)
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringIdle;

	// The value multiplied with the trigger input
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float TriggerStrength;

	// Increase or decrease strength the more we press the trigger (TriggerStrength or 1/TriggerStrength)
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	bool bDecreaseStrength;

	// Damping value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float Damping;

	// Force limit, 0 means no limit. 
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float ForceLimit;

	// An array the user can fill with grasps they want to use
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TArray<UMCGraspAnimDataAsset*> AnimationDataAssets;

	// Spring value during the actual grasp, this increases with the trigger input value
	float SpringActive;

	// Skeletal mesh to apply the animation on
	USkeletalMeshComponent* SkelComp;

	// Currently active grasp animation index
	int32 ActiveAnimIdx;

	// Currently active animation step size between frames (1 / nr of frames),
	// this will used to point in which frame we are relative to the trigger input
	float ActiveAnimStepSize;

	// True if the grasp trigger is released
	bool bIsIdle;

	// True if the grasp trigger is pulled until the end
	bool bIsMax;

	// Rotation where the motor will try to go to (map between the constraint instance and the target rotation)	
	FFrame DriveTarget;

	// Selected animation	
	FAnimation ActiveAnimation;
	
	// Animation list
	TArray<FAnimation> Animations;

	// Animation names
	TArray<FString> AnimationNames;
};
