// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspAnimController.h"
#include "Animation/SkeletalMeshActor.h"
#include "MCGraspAnimReader.h"

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

	SpringBase = 9000.f;
	SpringMultiplier = 5.f;
	Damping = 1000.f;
	ForceLimit = 0.f;
	CurrAnimGraspIndex = 0;
	bGraspIsActive = false;

	bFirstUpdate = true;
	bGrasIsWaitingInQueue = false;
}

// Called when the game starts
void UMCGraspAnimController::BeginPlay()
{
	Super::BeginPlay();

	// Init component
	Init();
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

// Called before begin play
bool UMCGraspAnimController::Init()
{
	if (!LoadSkeletalMesh())
	{
		return false;
	}

	if (!LoadAnimGrasps())
	{
		return false;
	}

	SetActiveGrasp();
	UE_LOG(LogTemp, Error, TEXT("%s::%d"), *FString(__func__), __LINE__);

	SetupInputBindings();
	UE_LOG(LogTemp, Error, TEXT("%s::%d"), *FString(__func__), __LINE__);

	return true;
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
					Constraint->SetAngularDriveParams(SpringBase, Damping, ForceLimit);
				}
				return true;
			}
		}
	}
	return false;
}

// Convert the animation grasps data assets into controller friendly values
bool UMCGraspAnimController::LoadAnimGrasps()
{
	for (const auto& DataAsset : AnimGraspDataAssets)
	{
		GraspAnims.Emplace(UMCGraspAnimReader::ConvertAssetToStruct(DataAsset));
	}
	return GraspAnims.Num() > 0;
}

// Bind user inputs for updating the grasps and switching the animations
void UMCGraspAnimController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAxis(InputAxisName, this, &UMCGraspAnimController::Update);
			IC->BindAction(InputNextAnimAction, IE_Pressed, this, &UMCGraspAnimController::NextAnim);
			IC->BindAction(InputPrevAnimAction, IE_Pressed, this, &UMCGraspAnimController::PrevAnim);
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

// Forward the axis input value to the grasp animation executor
void UMCGraspAnimController::Update(float Value)
{
	// Ensures the hand goes into the initial frame when this is called the first time
	if (bFirstUpdate)
	{
		if (ActiveGraspAnim.Frames.IsValidIndex(0))
		{
			DriveToHandOrientationTarget(ActiveGraspAnim.Frames[0]);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d Invalid index"), *FString(__func__), __LINE__);
			DriveToHandOrientationTarget(FMCGraspAnimFrameData());
		}
		bFirstUpdate = false;
	}

	if (Value > 0.05)
	{
		if (!bGraspIsActive)
		{
			bGraspIsActive = true;
		}

		// Calculate the new spring value of the motor
		NewSpringValue = SpringBase * ((SpringMultiplier * Value) + 1);

		// Calculate step size relative to the number of frames in the animation
		float StepSize = 1 / ((float) ActiveGraspAnim.Frames.Num() - 1);

		// Calculates how many steps we have passed, given then current input
		// When rounded down we know which step came before with this input
		float StepIteratorCountFloat = Value / StepSize;

		// We will be rounding down and a input of exactly 1 causes problems there
		if (Value >= 1)
		{
			StepIteratorCountFloat = 0.999999 / StepSize;
		}

		// Uses the float to int cast to round down
		int32 StepIteratorCountInt = (int32)StepIteratorCountFloat;

		// We calculate how far the input is past the step that came before it
		float NewInput = StepIteratorCountFloat - (float) StepIteratorCountInt;

		FMCGraspAnimFrameData StartFrame;
		if (ActiveGraspAnim.Frames.IsValidIndex(StepIteratorCountInt))
		{
			StartFrame = ActiveGraspAnim.Frames[StepIteratorCountInt];
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d"), *FString(__func__), __LINE__);
		}

		FMCGraspAnimFrameData EndFrame;
		if (ActiveGraspAnim.Frames.IsValidIndex(StepIteratorCountInt + 1))
		{
			EndFrame = ActiveGraspAnim.Frames[StepIteratorCountInt + 1];
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d"), *FString(__func__), __LINE__);
		}

		// Manipulate Orientation Drives
		FMCGraspAnimFrameData TargetOrientation;
		LerpHandOrientation(&TargetOrientation, StartFrame, EndFrame, NewInput);
		DriveToHandOrientationTarget(TargetOrientation);
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
void UMCGraspAnimController::NextAnim()
{
	if (CurrAnimGraspIndex >= AnimGraspDataAssets.Num()-1)
	{
		CurrAnimGraspIndex = 0;
	}
	else
	{
		CurrAnimGraspIndex++;
	}
	SetActiveGrasp();
}

// Switch to the previous animation
void UMCGraspAnimController::PrevAnim()
{
	if (CurrAnimGraspIndex <= 0)
	{
		CurrAnimGraspIndex = AnimGraspDataAssets.Num() - 1;
	} 
	else 
	{
		CurrAnimGraspIndex--;
	}
	SetActiveGrasp();
}



void UMCGraspAnimController::LerpHandOrientation(FMCGraspAnimFrameData* OutTarget, FMCGraspAnimFrameData StartFrame, FMCGraspAnimFrameData ClosedFrame, const float Input)
{
	TArray<FString> TempArray;
	StartFrame.Map.GenerateKeyArray(TempArray);
	for (FString S : TempArray) 
	{
		OutTarget->Map.Add(S, FMath::LerpRange(
			StartFrame.Map.Find(S)->AngularOrientationTarget,
			ClosedFrame.Map.Find(S)->AngularOrientationTarget, Input));
	}
}

void UMCGraspAnimController::DriveToHandOrientationTarget(const FMCGraspAnimFrameData& TargetFrame)
{
	FConstraintInstance* Constraint = nullptr;
	TArray<FString> TempArray;
	TargetFrame.Map.GenerateKeyArray(TempArray);
	for (FString S : TempArray) 
	{
		Constraint = BoneNameToConstraint(S);
		if (Constraint) 
		{
			Constraint->SetAngularDriveParams(NewSpringValue, Damping, ForceLimit);
			Constraint->SetAngularOrientationTarget(TargetFrame.Map.Find(S)->AngularOrientationTarget.Quaternion());
		}
	}
}

FConstraintInstance* UMCGraspAnimController::BoneNameToConstraint(FString BoneName)
{
	FConstraintInstance* Constraint = nullptr;

	//finds the constraint responsible for moving this bone
	for (FConstraintInstance* NewConstraint : SkelComp->Constraints)
	{
		if (NewConstraint->ConstraintBone1.ToString() == BoneName)
		{
			Constraint = NewConstraint;
		}
	}
	return Constraint;
}

void UMCGraspAnimController::StopGrasping()
{
	// Stop Grasp
	NewSpringValue = SpringBase;
	if (ActiveGraspAnim.Frames.IsValidIndex(0))
	{
		DriveToHandOrientationTarget(ActiveGraspAnim.Frames[0]);
	}
	else
	{
		DriveToHandOrientationTarget(FMCGraspAnimFrameData());
	}

	if (bGrasIsWaitingInQueue)
	{
		ActiveGraspAnim = QueuedGrasAnim;
		bGrasIsWaitingInQueue = false;
	}
	bGraspIsActive = false;
}

void UMCGraspAnimController::SetActiveGrasp()
{
	// if player is grasping put new grasp in queue, else change grasp immediately
	if (bGraspIsActive)
	{
		QueuedGrasAnim = GraspAnims[CurrAnimGraspIndex];
		bGrasIsWaitingInQueue = true;
		return;
	}
	ActiveGraspAnim = GraspAnims[CurrAnimGraspIndex];
}
