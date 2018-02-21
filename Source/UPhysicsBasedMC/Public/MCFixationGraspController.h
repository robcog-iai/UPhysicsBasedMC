// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshActor.h"
#include "MotionControllerComponent.h"
#include "SLRuntimeManager.h"
#include "MCFixationGraspController.generated.h"

/**
 * Functionality for grasping objects by fixation
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCFixationGraspController : public USphereComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UMCFixationGraspController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Init fixation grasp	
	void Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC, UInputComponent* InIC = nullptr);

private:
	// Bind grasping inputs
	void SetupInputBindings(UMotionControllerComponent* InMC, UInputComponent* InIC);

	// Try to fixate object to hand
	void TryToFixate();

	// Fixate object to hand
	void FixateObject(AStaticMeshActor* InSMA);

	// Detach fixation
	void TryToDetach();

	// Check if the static mesh actor can be grasped
	bool CanBeGrasped(AStaticMeshActor* InActor);

	// Function called when an item enters the fixation overlap area
	UFUNCTION()
	void OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Function called when an item leaves the fixation overlap area
	UFUNCTION()
	void OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Object maximum length (cm)
	UPROPERTY(EditAnywhere, Category = "MC")
	float ObjectMaxLength;

	// Object maximum mass (kg)
	UPROPERTY(EditAnywhere, Category = "MC")
	float ObjectMaxMass;

	// Flag if the object should be welded to the hand
	UPROPERTY(EditAnywhere, Category = "MC")
	bool bWeldFixation;

	// Hand to fixate (attach) the object to
	USkeletalMeshComponent* SkeletalHand;
	
	// Array of items currently in reach (overlapping the sphere component)
	TArray<AStaticMeshActor*> ObjectsInReach;

	// Fixated object
	AStaticMeshActor* FixatedObject;


	/* SemLog */
	// Start grasp event
	bool StartGraspEvent(AActor* OtherActor);

	// Finish grasp event
	bool FinishGraspEvent(AActor* OtherActor);

	// Semantic events runtime manager
	ASLRuntimeManager* SemLogRuntimeManager;

	// Current grasp event
	TSharedPtr<FOwlNode> GraspEvent;

	// Hand individual
	FOwlIndividualName HandIndividual;
};
