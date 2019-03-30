// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "Engine.h"
#include "MCGraspAnimStructs.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "MCGraspExecuter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMCGRASP_API UMCGraspExecuter : public UObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCGraspExecuter();
	
	// sets up all the motors
	void InitiateExecuter(ASkeletalMeshActor* Parent, const float& inSpringBase, const float& inSpringMultiplier, const float& inDamping, const float& inForceLimit);

	/*
	Loads the data of grasps into the grasping controller
	@param Data - data to load
	*/
	void SetGraspingData(FMCAnimationData Data);

	/*
	Responsible for updating the movement of all bones
	@param Hand - the AGraspingHand to move
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void UpdateGrasp(const float Input);

private:

	/*
	Calls LerpFingerOrientation on every finger
	@param Target - where calculated positions are saved
	@param Initial - initial hand state if input was 0
	@param Closed - final hand state if input was 1 
	@param Input - number from 0-1 that indicates how far the grasping trigger is being pushed down
	*/
	void LerpHandOrientation(FMCFrame* Target, FMCFrame Initial, FMCFrame Closed, const float Input);

	/*
	Calls DriveToFingerOrientationTarget for every finger.
	@param Target - the target position that has been calculated by lerp
	@param Hand - the AGraspingHand to move
	*/
	void DriveToHandOrientationTarget(FMCFrame* Target);

	/*
	Finds out which constraint belongs to which bone and sets them up  so we cann move them
	@param BoneName - name of bone
	@param Hand - AGraspingHand with the skeleton that has to be searched for the constraint
	*/
	FConstraintInstance* BoneNameToConstraint(FString BoneName);

	/*
	Stops the grasping process and resets the booleans that were changed
	@param Hand - AGraspingHand that should stop grasping
	*/
	void StopGrasping();

	// If hand is grasping
	bool bIsGrasping;

	// Current grasp loaded into hand
	FMCAnimationData GraspingData;

	// Spring value to be used for constraints
	// Makes angular motors more powerfull depending on how much the grasping button is pushed down
	float Spring;

	// When changing grasp type while grasping, the new grasp isn't applied immediately
	// Instead it is saved in this variable and applied once the user stops grasping 
	FMCAnimationData GraspQueue;

	// Set to true when there is a grasp waiting to be applied
	bool bIsInQueue;

	// the mesh that has to be moved
	ASkeletalMeshActor* Hand;

	// bool to check if this objject has been initialized correctly 
	bool bIsInitiated;

	// Minimum spring value
	float SpringBase = 9000;

	/**
	* Multiplier for the SpringBase. At an input of 1 the base value is multiplied by it.
	* At an input of 0.5 the base value is multiplied by half of it and so on. 
	*/ 
	float SpringMultiplier = 5;

	// Damping value
	float Damping = 1000;

	// Force limit. 0 means no limit. 
	float ForceLimit = 0;

	// bool that is used so the mesh goes into step 0 of it's current grasp when the game is started
	bool bFirstUpdate = true;
};
