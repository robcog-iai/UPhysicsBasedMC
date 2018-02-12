// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "MCFixationGrasp.generated.h"

/**
 * Functionality for grasping objects by fixation
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCFixationGrasp : public USphereComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UMCFixationGrasp();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Init fixation grasp	
	void Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC);

private:
	// Hand to fixate objects to
	USkeletalMeshComponent* Hand;

	// Bind grasping inputs
	void SetupInputBindings(UMotionControllerComponent* InMC);

	// Try to fixate object to hand
	void TryFixation();

	// Detach fixation
	void DetachFixation();

	// Function called when an item enters the fixation overlap area
	UFUNCTION()
	void OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Function called when an item leaves the fixation overlap area
	UFUNCTION()
	void OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
