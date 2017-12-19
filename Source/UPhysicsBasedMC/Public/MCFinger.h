// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "MCEnums.h"
#include "MCFinger.generated.h"

/**
*
*/
USTRUCT()
struct UPHYSICSBASEDMC_API FMCFinger
{
	GENERATED_USTRUCT_BODY()

	// Default constructor
	FMCFinger()
	{}

	// Finger type
	UPROPERTY(EditAnywhere, Category = "Finger")
	EMCFingerType FingerType;

	// Map of finger part to skeletal bone name
	UPROPERTY(EditAnywhere, Category = "Finger")
	TMap<EMCFingerPart, FString> FingerPartToBoneName;

	// Map of finger part to constraint
	TMap<EMCFingerPart, FConstraintInstance*> FingerPartToConstraint;

	// Set finger part to constraint from bone names
	bool SetFingerPartsConstraints(TArray<FConstraintInstance*>& Constraints)
	{
		// Iterate the bone names
		for (const auto& MapItr : FingerPartToBoneName)
		{
			// TODO null ptr exception if the names do not match
			// Check if bone name match with the constraint joint name
			FConstraintInstance* FingerPartConstraint = *Constraints.FindByPredicate(
				[&MapItr](FConstraintInstance* ConstrInst)
				{return ConstrInst->JointName.ToString() == MapItr.Value;}
			);
			// If constraint has been found, add to map
			if (FingerPartConstraint)
			{
				FingerPartToConstraint.Add(MapItr.Key, FingerPartConstraint);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Finger: Bone %s has no constraint!"), *MapItr.Value);
				return false;
			}
		}
		return true;
	}

	// Set constraint drive mode
	void SetFingerDriveMode(
		const EAngularDriveMode::Type DriveMode,
		const float InSpring,
		const float InDamping,
		const float InForceLimit)
	{
		for (const auto& MapItr : FingerPartToConstraint)
		{
			MapItr.Value->SetAngularDriveMode(DriveMode);
			if (DriveMode == EAngularDriveMode::TwistAndSwing)
			{
				MapItr.Value->SetOrientationDriveTwistAndSwing(true, true);
			}
			else if (DriveMode == EAngularDriveMode::SLERP)
			{
				MapItr.Value->SetOrientationDriveSLERP(true);
			}			
			MapItr.Value->SetAngularDriveParams(InSpring, InDamping, InForceLimit);
		}
	}
};