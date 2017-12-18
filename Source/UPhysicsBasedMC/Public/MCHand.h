// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "MCEnums.h"
#include "MCFinger.h"
#include "Engine/StaticMeshActor.h"
#include "SLRuntimeManager.h" // Semantic logger
#include "MCHand.generated.h"

/** Hand grasp constants */
enum
{
	NOT_GRASPABLE = 0,
	ONE_HAND_GRASPABLE = 1,
	TWO_HANDS_GRASPABLE = 2
};

/**
 * 
 */
UCLASS()
class UPHYSICSBASEDMC_API AMCHand : public ASkeletalMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor
	AMCHand();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Update the grasp //TODO state, power, step
	void UpdateGrasp(const float Goal);

	// Switch the grasping style
	void SwitchGrasp();
	
	// Fixation grasp via attachment of the object to the hand
	bool TryOneHandFixationGrasp();

	// Fixation grasp of two hands attachment
	bool TryTwoHandsFixationGrasp();

	// Fixation grasp of two hands attachment (triggered by other hand)
	void TwoHandsFixationGraspFromOther();

	// Detach fixation grasp from hand
	bool DetachFixationGrasp();

	// Detach fixation grasp from hand (triggered by the other hand)
	bool DetachTwoHandFixationGraspFromOther();

	// Get possible two hand grasp object
	AStaticMeshActor* GetTwoHandsGraspableObject() const { return TwoHandsGraspableObject; };

	// Check if the two hand grasp is still valid (the hands have not moved away from each other)
	bool IsTwoHandGraspStillValid();

	// Set pointer to other hand, used for two hand fixation grasp
	void SetOtherHand(AMCHand* InOtherHand);

	// Hand type
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hand")
	EMCHandType HandType;

	// Thumb finger skeletal bone names
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hand")
	FMCFinger Thumb;

	// Index finger skeletal bone names
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hand")
	FMCFinger Index;

	// Middle finger skeletal bone names
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hand")
	FMCFinger Middle;

	// Ring finger skeletal bone names
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hand")
	FMCFinger Ring;

	// Pinky finger skeletal bone names
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hand")
	FMCFinger Pinky;

	// Flag showing that the hand is ready for a two hands grasp
	bool bReadyForTwoHandsGrasp;

private:
	// Setup player input bindings
	void SetupInputBindings();

	// Post edit change property callback
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	// Check if the object in reach is one-, two-hand(s), or not graspable
	UFUNCTION()
	void OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Object out or grasping reach, remove as possible grasp object
	UFUNCTION()
	void OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Start grasp event
	bool StartGraspEvent(AActor* OtherActor);

	// Finish grasp event
	bool FinishGraspEvent(AActor* OtherActor);

	// Check if object is graspable, return the number of hands (0, 1, 2)
	uint8 CheckObjectGraspableType(AActor* InActor);

	// Hold grasp in the current position
	void MaintainFingerPositions();

	// Setup hand default values
	void SetupHandDefaultValues(EMCHandType HandType);

	// Setup skeletal mesh default values
	void SetupSkeletalDefaultValues(USkeletalMeshComponent* InSkeletalMeshComponent);

	// Setup fingers angular drive values
	void SetupAngularDriveValues(EAngularDriveMode::Type DriveMode);

	// Attach to left hand
	void TryFixationGrasp();

	// Detach from left hand
	void TryFixationGraspDetach();

	// Enable grasping with fixation
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp")
	bool bFixationGraspEnabled;

	// Enable two hand grasping with fixation
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp", meta = (editcondition = "bFixationGraspEnabled"))
	bool bTwoHandsFixationGraspEnabled;

	// Collision component used for attaching grasped objects
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp", meta = (editcondition = "bFixationGraspEnabled"))
	USphereComponent* FixationGraspArea;

	// Maximum mass (kg) of an object that can be attached to the hand
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp", meta = (editcondition = "bFixationGraspEnabled"), meta = (ClampMin = 0))
	float OneHandFixationMaximumMass;

	// Maximum length (cm) of an object that can be attached to the hand
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp", meta = (editcondition = "bFixationGraspEnabled"), meta = (ClampMin = 0))
	float OneHandFixationMaximumLength;

	// Maximum mass (kg) of an object that can be attached to two hands
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp", meta = (editcondition = "bFixationGraspEnabled"), meta = (ClampMin = 0))
	float TwoHandsFixationMaximumMass;

	// Maximum length (cm) of an object that can be attached to two hands
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Fixation Grasp", meta = (editcondition = "bFixationGraspEnabled"), meta = (ClampMin = 0))
	float TwoHandsFixationMaximumLength;

	// Spring value to apply to the angular drive (Position strength)
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Drive Parameters")
	TEnumAsByte<EAngularDriveMode::Type> AngularDriveMode;;

	// Spring value to apply to the angular drive (Position strength)
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Drive Parameters", meta = (ClampMin = 0))
	float Spring;

	// Damping value to apply to the angular drive (Velocity strength) 
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Drive Parameters", meta = (ClampMin = 0))
	float Damping;

	// Limit of the force that the angular drive can apply
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Drive Parameters", meta = (ClampMin = 0))
	float ForceLimit;

	// Objects that are in reach to be grasped by one hand
	TArray<AStaticMeshActor*> OneHandGraspableObjects;

	// Pointer to the grasped object
	AStaticMeshActor* OneHandGraspedObject;

	// Object that is in reach, and is two hand graspable
	AStaticMeshActor* TwoHandsGraspableObject;

	// Pointer to the object grasped by with two hands
	AStaticMeshActor* TwoHandsGraspedObject;

	// Pointer to the other hand (used for two hand fixation grasp)
	AMCHand* OtherHand;

	// If the hand is mimicking movements in the two hand fixation grasp case (no actual attachment)
	bool bMovementMimickingHand;

	// Movement mimicking relative location from the other hand
	FVector MimickingRelativeLocation;

	// Movement mimicking relative rotation from the other hand
	FQuat MimickingRelativeRotation;

	// Mark that the grasp has been held, avoid reinitializing the finger drivers
	bool bGraspHeld;

	// Hand individual
	FOwlIndividualName HandIndividual;

	// Semantic events runtime manager
	ASLRuntimeManager* SemLogRuntimeManager;

	// Current grasp event
	TSharedPtr<FOwlNode> GraspEvent;
};
