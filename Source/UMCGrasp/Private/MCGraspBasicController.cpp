// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspBasicController.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UMCGraspBasicController::UMCGraspBasicController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bIgnore = false;

	// Default parameters
	HandType = EMCHandType::Left;
	InputAxisName = "LeftGrasp";
	SkeletalType = EMCSkeletalType::Default;

	// Driver parameters
	AngularDriveMode = EAngularDriveMode::SLERP;
	Spring = 50000.0f;
	Damping = 500.0f;
	ForceLimit = 250000.0f;

	// Previous input value
	PrevInputVal = 0.f;
	MaxAngleMultiplier = 55.f;
}

// Called when the game starts
void UMCGraspBasicController::BeginPlay()
{
	Super::BeginPlay();

	// Check if the controller can be init to the skeletal mesh
	Init();
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCGraspBasicController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCGraspBasicController, HandType))
	{
		if (HandType == EMCHandType::Left)
		{
			InputAxisName = "LeftGrasp";
		}
		else if (HandType == EMCHandType::Right)
		{
			InputAxisName = "RightGrasp";
		}
	}
}
#endif // WITH_EDITOR

// Init the controller
void UMCGraspBasicController::Init()
{
	if (bIgnore)
	{
		return;
	}

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
					if (SkeletalType == EMCSkeletalType::Default)
					{
						IC->BindAxis(InputAxisName, this, &UMCGraspBasicController::Update);
					}
					else if (SkeletalType == EMCSkeletalType::IAI)
					{
						IC->BindAxis(InputAxisName, this, &UMCGraspBasicController::Update_IAI);
					}
					else if (SkeletalType == EMCSkeletalType::Genesis)
					{
						IC->BindAxis(InputAxisName, this, &UMCGraspBasicController::Update_Genesis);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("%s::%d No Input Component found.."), *FString(__func__), __LINE__);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s::%d No Player controller found.."), *FString(__func__), __LINE__);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d No valid skeletal mesh component found.."), *FString(__func__), __LINE__);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Owner is not a skeletal mesh actor.."), *FString(__func__), __LINE__);
	}
}

// Update the grasp
void UMCGraspBasicController::Update(float Value)
{
	// Skip iterating constraints for small changes
	if (FMath::Abs(Value - PrevInputVal) > 0.025)
	{
		PrevInputVal = Value;
		// Apply target to fingers
		for (auto& ConstraintInstance : SkeletalMesh->Constraints)
		{
			ConstraintInstance->SetAngularOrientationTarget(FRotator(0.f, 0.f, Value * MaxAngleMultiplier).Quaternion());
		}
	}
}

// Update the grasp as a IAI Hand
void UMCGraspBasicController::Update_IAI(float Value)
{
	// Skip iterating constraints for small changes
	if (FMath::Abs(Value - PrevInputVal) > 0.025)
	{
		PrevInputVal = Value;
		// Apply target to fingers
		for (auto& ConstraintInstance : SkeletalMesh->Constraints)
		{
			if (ConstraintInstance->ConstraintBone1.ToString().Contains("thumb"))
			{
				//ConstraintInstance->SetAngularOrientationTarget(FRotator(0.f, Value * MaxAngleMultiplier, Value * MaxAngleMultiplier).Quaternion());
			}
			else
			{
				ConstraintInstance->SetAngularOrientationTarget(FRotator(0.f, 0.f, Value * MaxAngleMultiplier).Quaternion());
			}
		}
	}
}

// Update the grasp for the genesis skeleton
void UMCGraspBasicController::Update_Genesis(float Value)
{
	// Skip iterating constraints for small changes
	if (FMath::Abs(Value - PrevInputVal) > 0.05)
	{
		PrevInputVal = Value;

		if (HandType == EMCHandType::Right)
		{
			Value *= -1.f;
			//UE_LOG(LogTemp, Warning, TEXT("%s::%d"), *FString(__func__), __LINE__);
		}

		// Apply target to fingers
		for (auto& ConstraintInstance : SkeletalMesh->Constraints)
		{
			if (ConstraintInstance->ConstraintBone1.ToString().Contains("Carpal"))
			{
				continue;
			}
			//else if (ConstraintInstance->ConstraintBone1.ToString().Contains("Thumb"))
			//{
			//	ConstraintInstance->SetAngularOrientationTarget(FRotator( - Value * MaxAngleMultiplier, 0.f, 0.f).Quaternion());
			//}
			else
			{
				ConstraintInstance->SetAngularOrientationTarget(FRotator(0.f, Value * MaxAngleMultiplier, 0.f).Quaternion());
			}
		}
	}
}
