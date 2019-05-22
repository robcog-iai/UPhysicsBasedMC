// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MCGraspAnimDataAsset.h"
#include "MCGraspAnimStructs.h"
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
	// Init the component
	bool Init();

	// Prepare the skeletal mesh component physics and angular motors
	bool LoadSkeletalMesh();

	// Convert the animation grasps into controller friendly values
	bool LoadAnimGrasps();

	// Bind user inputs for updating the grasps and switching the animations
	void SetupInputBindings();

	// Update the grasp animation from the trigger input
	void Update(float Value);

	// Switch to the next grasp animation
	void NextAnim();

	// Switch to the previous animation
	void PrevAnim();


private:
	/*
	Uses interpolation to calculate target orientation for bones
	@param Target - where calculated positions are saved
	@param Initial - initial hand state if input was 0
	@param Closed - final hand state if input was 1
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void LerpHandOrientation(FMCGraspAnimFrameData* OutTarget,
		const FMCGraspAnimFrameData& Frame1,
		const FMCGraspAnimFrameData& Frame2,
		float Alpha);

	/*
	Sets all the constraints orientation drives to go into target orientation
	@param Target - the target position that has been calculated by lerp
	*/
	void DriveToHandOrientationTarget(const FMCGraspAnimFrameData& Target);

	/*
	Finds out which constraint belongs to which bone
	@param BoneName - name of bone
	*/
	FConstraintInstance* BoneNameToConstraint(FString BoneName);

	/*
	Stops the grasping process and resets the booleans that were changed
	*/
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
	TArray<UMCGraspAnimDataAsset*> AnimGraspDataAssets;

	// Minimum spring value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringBase;

	// Increase the strength of the spring value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringMultiplier;

	// Damping value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float Damping;

	// Force limit, 0 means no limit. 
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float ForceLimit;
	
	// Controller friendly conversion of the animation grasps
	TArray<FMCGraspAnimData> GraspAnims;

	// Skeletal mesh to apply the animation on
	USkeletalMeshComponent* SkelComp;

	// Currently active grasp animation index
	int32 CurrAnimGraspIndex;

	// Flag showing inf the grasp is ongoing
	bool bGraspIsActive;

	// Spring value 
	float NewSpringValue;




	// Current grasp loaded into hand
	FMCGraspAnimData ActiveGraspAnim;

	// When changing grasp type while grasping, the new grasp isn't applied immediately
	// Instead it is saved in this variable and applied once the user stops grasping 
	FMCGraspAnimData QueuedGrasAnim;

	// Set to true when there is a grasp waiting to be applied
	bool bGrasIsWaitingInQueue;

	// bool that is used so the mesh goes into step 0 of it's current grasp when the game is started
	bool bFirstUpdate;
};
