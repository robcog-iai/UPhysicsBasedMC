// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspAnimController.h"
#include "Animation/SkeletalMeshActor.h"

// Sets default values for this component's properties
UMCGraspAnimController::UMCGraspAnimController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITOR
	HandType = EMCGraspAnimHandType::Left;
#endif // WITH_EDITOR

	// Default parameters
	InputAxisName = "LeftGrasp";
	InputNextAnimAction = "LeftNextGraspAnim";
	InputPrevAnimAction = "LeftPrevGraspAnim";

	SpringIdle = 9000.f;
	SpringActiveMultiplier = 5.f;
	Damping = 1000.f;
	ForceLimit = 0.f;
	ActiveAnimIndex = INDEX_NONE;
	
	
	bGraspIsActive = false;
	bFirstUpdate = true;
	bGrasIsWaitingInQueue = false;
}

// Called when the game starts
void UMCGraspAnimController::BeginPlay()
{
	Super::BeginPlay();

	// Check if the skeletal mesh is valid and animations are loaded
	if (Init())
	{
		// Bind the user input to the callbacks
		//SetupInputBindings();

		// The active spring value is at least the value of the idle
		SpringActive = SpringIdle;

		// Go to the first animation mode
		GotoFirstAnimation();
	}
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCGraspAnimController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCGraspAnimController, HandType))
	{
		if (HandType == EMCGraspAnimHandType::Left)
		{
			InputAxisName = "LeftGrasp";
			InputNextAnimAction = "LeftNextGraspAnim";
			InputPrevAnimAction = "LeftPrevGraspAnim";
		}
		else if (HandType == EMCGraspAnimHandType::Right)
		{
			InputAxisName = "RightGrasp";
			InputNextAnimAction = "RightNextGraspAnim";
			InputPrevAnimAction = "RightPrevGraspAnim";
		}
	}
}
#endif // WITH_EDITOR

// Init component, return false is something went wrong
bool UMCGraspAnimController::Init()
{
	return LoadSkeletalMesh() && LoadAnimationData();
}

// Prepare the skeletal mesh component physics and angular motors
bool UMCGraspAnimController::LoadSkeletalMesh()
{
	if (ASkeletalMeshActor* ParentAsSkMA = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		SkelComp = ParentAsSkMA->GetSkeletalMeshComponent();
		if (SkelComp)
		{
			SkelComp->SetSimulatePhysics(true);
			SkelComp->SetEnableGravity(false);

			if (SkelComp->GetPhysicsAsset())
			{
				//sets up the constraints so they can be moved 
				for (FConstraintInstance* Constraint : SkelComp->Constraints)
				{
					Constraint->SetAngularSwing1Limit(ACM_Free, 0);
					Constraint->SetAngularSwing2Limit(ACM_Free, 0);
					Constraint->SetAngularTwistLimit(ACM_Free, 0);
					Constraint->SetAngularVelocityDriveTwistAndSwing(false, false);
					Constraint->SetAngularVelocityDriveSLERP(true);
					Constraint->SetAngularDriveMode(EAngularDriveMode::SLERP);
					Constraint->SetAngularDriveParams(SpringIdle, Damping, ForceLimit);
				}
				return true;
			}
		}
	}
	return false;
}

// Load the data from the animation data assets in a more optimized form, return true if at least one animation is loaded
bool UMCGraspAnimController::LoadAnimationData()
{	
	// Remove any unset references in the array
	AnimationDataAssets.Remove(nullptr);

	for (const auto& Animation : AnimationDataAssets)
	{
		// Iterate frames from the animation
		FAnimation CurrAnim;
		for (const auto& Frame : Animation->Frames)
		{
			// Iterate data from the frame and cache it
			FFrame CurrFrame;
			for (const auto& BoneData : Frame.BonesData)
			{
				if (FConstraintInstance* CI = SkelComp->FindConstraintInstance(FName(*BoneData.Key)))
				{
					CurrFrame.Add(CI, BoneData.Value.AngularOrientationTarget);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find constraint %s"), *FString(__func__), __LINE__, *BoneData.Key);
				}
			}
			CurrAnim.Add(CurrFrame);
		}
		Animations.Add(CurrAnim);
	}
	return Animations.Num() > 0;
}

// Set first grasp animation
void UMCGraspAnimController::GotoFirstAnimation()
{
	ActiveAnimIndex = 0;
	ActiveAnimation = Animations[0];
	SetTargetToIdle();
	DriveToTarget();
}

// Bind user inputs for updating the grasps and switching the animations
void UMCGraspAnimController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAxis(InputAxisName, this, &UMCGraspAnimController::GraspUpdateCallback);
			IC->BindAction(InputNextAnimAction, IE_Pressed, this, &UMCGraspAnimController::GotoNextAnimationCallback);
			IC->BindAction(InputPrevAnimAction, IE_Pressed, this, &UMCGraspAnimController::GotoPreviousAnimationCallback);
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

// Set the cached target to the first frame 
void UMCGraspAnimController::SetTargetToIdle()
{
	DriveTarget = ActiveAnimation[0];
}

// Compute and set the cached target by interpolating between the two frames
void UMCGraspAnimController::SetTargetUsingLerp(const FFrame& FrameA, const FFrame& FrameB, float Alpha)
{
	for (auto& DT : DriveTarget)
	{
		DT.Value = FMath::LerpRange(FrameA[DT.Key], FrameB[DT.Key], Alpha);
	}
}

// Set the drive parameters to the cached target
void UMCGraspAnimController::DriveToTarget()
{
	for (const auto& DT : DriveTarget)
	{
		DT.Key->SetAngularDriveParams(SpringActive, Damping, ForceLimit);
		DT.Key->SetAngularOrientationTarget(DT.Value.Quaternion());
	}
}

/* Input callbacks */
// Forward the axis input value to the grasp animation executor
void UMCGraspAnimController::GraspUpdateCallback(float Value)
{
	// Ensures the hand goes into the initial frame when this is called the first time
	if (bFirstUpdate)
	{
		SetTargetToIdle();
		DriveToTarget();
		bFirstUpdate = false;
	}

	if (Value > 0.05)
	{
		if (!bGraspIsActive)
		{
			bGraspIsActive = true;
		}

		// Calculate the new spring value of the motor
		SpringActive = SpringIdle + (SpringIdle * SpringActiveMultiplier * Value);

		// Calculate step size relative to the number of frames in the animation
		float StepSize = 1 / ((float) ActiveAnimation.Num() - 1);

		// Calculates how many steps we have passed, given then current input
		// When rounded down we know which step came before with this input
		float StepIteratorCountFloat = Value / StepSize;

		// We will be rounding down and a input of exactly 1 causes problems there
		if (Value >= 1)
		{
			StepIteratorCountFloat = 0.999999 / StepSize;
		}

		// Uses the float to int cast to round down
		int32 FrameIndex = (int32)StepIteratorCountFloat;

		// We calculate how far the input is past the step that came before it
		float Alpha = StepIteratorCountFloat - (float) FrameIndex;

		SetTargetUsingLerp(ActiveAnimation[FrameIndex], ActiveAnimation[FrameIndex + 1], Alpha);
		DriveToTarget();
	}
	else
	{
		if (bGraspIsActive)
		{
			// Stop grasping
			StopGrasping();
		}
	}
}

// Switch to the next grasp animation
void UMCGraspAnimController::GotoNextAnimationCallback()
{
	if (ActiveAnimIndex >= AnimationDataAssets.Num()-1)
	{
		ActiveAnimIndex = 0;
	}
	else
	{
		ActiveAnimIndex++;
	}
	SetActiveGrasp();
}

// Switch to the previous animation
void UMCGraspAnimController::GotoPreviousAnimationCallback()
{
	if (ActiveAnimIndex <= 0)
	{
		ActiveAnimIndex = AnimationDataAssets.Num() - 1;
	} 
	else 
	{
		ActiveAnimIndex--;
	}
	SetActiveGrasp();
}



void UMCGraspAnimController::StopGrasping()
{
	// Stop Grasp
	SpringActive = SpringIdle;
	//if (ActiveAnimDA->Frames.IsValidIndex(0))
	//DriveToHandOrientationTarget(ActiveAnimDA->Frames[0]);

	SetTargetToIdle();
	DriveToTarget();


	if (bGrasIsWaitingInQueue)
	{
		//ActiveAnim = QueuedAnim;
		//ActiveAnimDA = QueuedAnimDA;
		bGrasIsWaitingInQueue = false;
	}
	bGraspIsActive = false;
}

void UMCGraspAnimController::SetActiveGrasp()
{
	// if player is grasping put new grasp in queue, else change grasp immediately
	if (bGraspIsActive)
	{
		//QueuedAnimDA = AnimationDataAssets[ActiveAnimIndex];
		bGrasIsWaitingInQueue = true;
		return;
	}
	//ActiveAnimDA = AnimationDataAssets[ActiveAnimIndex];


}
