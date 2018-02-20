// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspController.h"
#include "PhysicsEngine/ConstraintInstance.h"

// Constructor
UMCGraspController::UMCGraspController()
{
	// Angular drive default values
	AngularDriveMode = EAngularDriveMode::SLERP;
	Spring = 190000.0f;
	Damping = 100.0f;
	ForceLimit = 900000.0f;
	UpdateMultiplier = 100.f;
}

// Init grasp controller
void UMCGraspController::Init(USkeletalMeshComponent* InHand, EControllerHand InHandType, UInputComponent* InIC)
{
	// Set pointer of skeletal hand
	SkeletalHand = InHand;

	// Set handedness
	HandType = InHandType;

	// Check if InputComponent is given
	if (InIC)
	{
		SetupInputBindings(InIC);
	}
	else
	{
		// Get the input controller for mapping the grasping control inputs
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			UInputComponent* IC = PC->InputComponent;
			if (IC)
			{
				SetupInputBindings(IC);
			}
		}
	}

	// Setup fingers
	SetupFingers();
}

// Setup input bindings
void UMCGraspController::SetupInputBindings(UInputComponent* InIC)
{	
	// Check hand type
	if (HandType == EControllerHand::Left)
	{
		InIC->BindAxis("LeftGrasp", this, &UMCGraspController::Update);
	}
	else if (HandType == EControllerHand::Right)
	{
		InIC->BindAxis("RightGrasp", this, &UMCGraspController::Update);
	}
}

// Setup fingers
void UMCGraspController::SetupFingers()
{
	// Local struct for storing bone names and their bone type
	struct FBoneNameAndType
	{
		FBoneNameAndType(FMCFingerBone* InFingerBone, const FString& InBoneName/*, EFingerBone InBoneType*/) :
			FingerBone(InFingerBone), BoneName(InBoneName)/*, BoneType(InBoneType)*/{};
		FMCFingerBone* FingerBone;
		FString BoneName;
		/*EFingerBone BoneType;*/
	};
	// Array of the bone names and their types
	TArray<FBoneNameAndType> BonesAndTypes;
	
	// Set the postfix of hand type
	const FString HandTypePostfix = HandType == EControllerHand::Left ? FString("l") : FString("r");

	BonesAndTypes.Add(FBoneNameAndType(&Thumb.Distal, FString("thumb_03_").Append(HandTypePostfix)/*, EFingerBone::Distal*/));
	BonesAndTypes.Add(FBoneNameAndType(&Thumb.Intermediate, FString("thumb_02_").Append(HandTypePostfix)/*, EFingerBone::Intermediate*/));
	BonesAndTypes.Add(FBoneNameAndType(&Thumb.Proximal, FString("thumb_01_").Append(HandTypePostfix)/*, EFingerBone::Proximal*/));

	BonesAndTypes.Add(FBoneNameAndType(&Index.Distal, FString("index_03_").Append(HandTypePostfix)/*, EFingerBone::Distal*/));
	BonesAndTypes.Add(FBoneNameAndType(&Index.Intermediate, FString("index_02_").Append(HandTypePostfix)/*, EFingerBone::Intermediate*/));
	BonesAndTypes.Add(FBoneNameAndType(&Index.Proximal, FString("index_01_").Append(HandTypePostfix)/*, EFingerBone::Proximal*/));

	BonesAndTypes.Add(FBoneNameAndType(&Middle.Distal, FString("middle_03_").Append(HandTypePostfix)/*, EFingerBone::Distal*/));
	BonesAndTypes.Add(FBoneNameAndType(&Middle.Intermediate, FString("middle_02_").Append(HandTypePostfix)/*, EFingerBone::Intermediate*/));
	BonesAndTypes.Add(FBoneNameAndType(&Middle.Proximal, FString("middle_01_").Append(HandTypePostfix)/*, EFingerBone::Proximal*/));

	BonesAndTypes.Add(FBoneNameAndType(&Ring.Distal, FString("ring_03_").Append(HandTypePostfix)/*, EFingerBone::Distal*/));
	BonesAndTypes.Add(FBoneNameAndType(&Ring.Intermediate, FString("ring_02_").Append(HandTypePostfix)/*, EFingerBone::Intermediate*/));
	BonesAndTypes.Add(FBoneNameAndType(&Ring.Proximal, FString("ring_01_").Append(HandTypePostfix)/*, EFingerBone::Proximal*/));

	BonesAndTypes.Add(FBoneNameAndType(&Pinky.Distal, FString("pinky_03_").Append(HandTypePostfix)/*, EFingerBone::Distal*/));
	BonesAndTypes.Add(FBoneNameAndType(&Pinky.Intermediate, FString("pinky_02_").Append(HandTypePostfix)/*, EFingerBone::Intermediate*/));
	BonesAndTypes.Add(FBoneNameAndType(&Pinky.Proximal, FString("pinky_01_").Append(HandTypePostfix)/*, EFingerBone::Proximal*/));

	// Iterate array and setup fingers
	for (auto& BoneAndTypeItr : BonesAndTypes)
	{
		if (FConstraintInstance* FingerConstraint = GetFingerConstraint(BoneAndTypeItr.BoneName))
		{
			BoneAndTypeItr.FingerBone->Init(/*BoneAndTypeItr.BoneType, */BoneAndTypeItr.BoneName, FingerConstraint);

			FingerConstraint->SetAngularDriveMode(AngularDriveMode);
			if (AngularDriveMode == EAngularDriveMode::TwistAndSwing)
			{
				FingerConstraint->SetOrientationDriveTwistAndSwing(true, true);
			}
			else if (AngularDriveMode == EAngularDriveMode::SLERP)
			{
				FingerConstraint->SetOrientationDriveSLERP(true);
			}
			FingerConstraint->SetAngularDriveParams(Spring, Damping, ForceLimit);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] Could not find ConstraintInstance for bone %s"), *BoneAndTypeItr.BoneName)
		}
	}
}

// Get finger constraint
FConstraintInstance* UMCGraspController::GetFingerConstraint(const FString& BoneName)
{
	return *SkeletalHand->Constraints.FindByPredicate(
		[&BoneName](FConstraintInstance* ConstrInst) {return ConstrInst->JointName.ToString() == BoneName; }
	);
}

// Update grasp
void UMCGraspController::Update(const float Val)
{
	Thumb.Distal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Thumb.Intermediate.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Thumb.Proximal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));

	Index.Distal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Index.Intermediate.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Index.Proximal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));

	Middle.Distal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Middle.Intermediate.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Middle.Proximal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));

	Ring.Distal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Ring.Intermediate.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Ring.Proximal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));

	Pinky.Distal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Pinky.Intermediate.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
	Pinky.Proximal.ConstraintInstance->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Val * UpdateMultiplier)));
}
