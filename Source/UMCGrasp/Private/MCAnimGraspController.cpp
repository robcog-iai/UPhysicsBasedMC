// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCAnimGraspController.h"
#include "Animation/SkeletalMeshActor.h"
#include "MCAnimGraspReader.h"

// Sets default values for this component's properties
UMCAnimGraspController::UMCAnimGraspController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

#if WITH_EDITOR
	HandType = EMCAnimGraspHandType::Left;
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
}

// Called when the game starts
void UMCAnimGraspController::BeginPlay()
{
	Super::BeginPlay();

	// Init component
	Init();
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCAnimGraspController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCAnimGraspController, HandType))
	{
		if (HandType == EMCAnimGraspHandType::Left)
		{
			InputAxisName = "LeftGrasp";
			InputNextAnimAction = "LeftNextGraspAnim";
			InputPrevAnimAction = "LeftPrevGraspAnim";
		}
		else if (HandType == EMCAnimGraspHandType::Right)
		{
			InputAxisName = "RightGrasp";
			InputNextAnimAction = "RightNextGraspAnim";
			InputPrevAnimAction = "RightPrevGraspAnim";
		}
	}
}
#endif // WITH_EDITOR

// Called before begin play
void UMCAnimGraspController::Init()
{
	if (ASkeletalMeshActor* ParentAsSkMA = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		if (AnimGraspDataAssets.Num() > 0)
		{
			// Create and initialize the grasp executor
			AnimGraspExecutor = NewObject<UMCAnimGraspExec>();
			if (AnimGraspExecutor)
			{
				AnimGraspExecutor->Init(ParentAsSkMA, SpringBase, SpringMultiplier, Damping, ForceLimit);
				AnimGraspExecutor->LoadGrasp(AnimGraspDataAssets[CurrAnimGraspIndex]);

				// Bind user inputs
				SetupInputBindings();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d No grasps animations loaded.."), *FString(__func__), __LINE__);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Owner is not a skeletal actor"), *FString(__func__), __LINE__);
	}
}

void UMCAnimGraspController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAxis(InputAxisName, this, &UMCAnimGraspController::Update);
			IC->BindAction(InputNextAnimAction, IE_Pressed, this, &UMCAnimGraspController::NextAnim);
			IC->BindAction(InputPrevAnimAction, IE_Pressed, this, &UMCAnimGraspController::PrevAnim);
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
void UMCAnimGraspController::Update(float Val)
{
	AnimGraspExecutor->UpdateGrasp(Val);
}

// Switch to the next grasp animation
void UMCAnimGraspController::NextAnim()
{
		if (CurrAnimGraspIndex >= AnimGraspDataAssets.Num()-1)
		{
			CurrAnimGraspIndex = 0;
		}
		else
		{
			CurrAnimGraspIndex++;
		}

		AnimGraspExecutor->LoadGrasp(AnimGraspDataAssets[CurrAnimGraspIndex]);
}

// Switch to the previous animation
void UMCAnimGraspController::PrevAnim()
{
	if (CurrAnimGraspIndex <= 0)
	{
		CurrAnimGraspIndex = AnimGraspDataAssets.Num() - 1;
	} else 
	{
		CurrAnimGraspIndex--;
	}

	AnimGraspExecutor->LoadGrasp(AnimGraspDataAssets[CurrAnimGraspIndex]);
}