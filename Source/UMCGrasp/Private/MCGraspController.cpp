// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspController.h"

#define MC_MAX_ANGULAR_TARGET 45.f

// Sets default values for this component's properties
UMCGraspController::UMCGraspController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Default parameters
	HandType = EMCGraspHandType::Left;
	InputAxisName = "LeftGrasp";

	// Driver parameters
	AngularDriveMode = EAngularDriveMode::SLERP;
	Spring = 15000.0f;
	Damping = 100.0f;
	ForceLimit = 50000.0f;
}

// Called when the game starts
void UMCGraspController::BeginPlay()
{
	Super::BeginPlay();

	// Check that owner is a skeletal mesh actor and has a valid skeletal mesh component
	if (ASkeletalMeshActor* OwnerAsSkelMA = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		// Get owner skeletal mesh component
		SkeletalMesh = OwnerAsSkelMA->GetSkeletalMeshComponent();
		if (SkeletalMesh)
		{
			// Enable physics, disable gravity
			SkeletalMesh->SetMobility(EComponentMobility::Movable);
			SkeletalMesh->SetSimulatePhysics(true);

			// Setup angular driver
			for (auto& ConstraintInstance : SkeletalMesh->Constraints)
			{
				ConstraintInstance->SetAngularDriveMode(AngularDriveMode);
				ConstraintInstance->SetAngularDriveParams(Spring, Damping, ForceLimit);
			}

			// Set user input bindings
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				if (UInputComponent* IC = PC->InputComponent)
				{
					IC->BindAxis(InputAxisName, this, &UMCGraspController::Update);
				}
			}
		}
	}
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCGraspController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCGraspController, HandType))
	{
		if (HandType == EMCGraspHandType::Left)
		{
			InputAxisName = "LeftGrasp";
		}
		else if (HandType == EMCGraspHandType::Right)
		{
			InputAxisName = "RightGrasp";
		}
	}
}
#endif // WITH_EDITOR

// Update the grasp
void UMCGraspController::Update(float Value)
{	
	// Apply target to fingers
	for (auto& ConstraintInstance : SkeletalMesh->Constraints)
	{
		ConstraintInstance->SetAngularOrientationTarget(FRotator(0.f, 0.f, Value * MC_MAX_ANGULAR_TARGET).Quaternion());
	}
}
