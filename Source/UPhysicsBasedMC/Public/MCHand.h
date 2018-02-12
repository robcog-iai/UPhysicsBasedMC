// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "MCHandMovement.h"
#include "MCHandGrasp.h"
#include "MCHand.generated.h"

/**
 * Hand component with movement and grasping controller
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCHand : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for the component
	UMCHand();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Init hand with the motion controllers
	void Init(UMotionControllerComponent* InMC);

private:
	// Location PID controller
	UPROPERTY(EditAnywhere, Category = "MC|Movement Control")
	FMCHandMovement MovementController;

	UPROPERTY(EditAnywhere, Category = "MC|Grasp Control")
	FMCHandGrasp GraspController;
};
