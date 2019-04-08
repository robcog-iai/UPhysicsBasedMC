// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspController.h"

// Sets default values for this component's properties
UMCGraspController::UMCGraspController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// Default parameters
	HandType = EMCGraspHandType::Left;
	GraspAction = "LeftGrasp";
	NextGraspAction = "LeftNext";
	PreviousGraspAction = "LeftPrevious";
}


// Called when the game starts
void UMCGraspController::BeginPlay()
{
	Super::BeginPlay();

	if (ASkeletalMeshActor* ParentActor = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		SetupInputBindings();
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
			GraspAction = "LeftGrasp";
			NextGraspAction = "LeftNext";
			PreviousGraspAction = "LeftPrevious";
		}
		else if (HandType == EMCGraspHandType::Right)
		{
			GraspAction = "RightGrasp";
			NextGraspAction = "RightNext";
			PreviousGraspAction = "RightPrevious";
		}
	}
}
#endif // WITH_EDITOR

void UMCGraspController::InitializeComponent()
{
	Super::InitializeComponent();

	//Create GraspExecuter
	if (ASkeletalMeshActor* ParentActor = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		if (EquippedGrasps.Num() > 0)
		{
		GraspExecuter = NewObject<UMCGraspExecuter>();
		GraspExecuter->SetGraspingData(UMCGraspAnimReader::ConvertAssetToStruct(EquippedGrasps[CurrentGrasp]));
		GraspExecuter->InitiateExecuter(ParentActor, SpringBase, SpringMultiplier, Damping, ForceLimit);
		}
	}
}

void UMCGraspController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAxis(GraspAction, this, &UMCGraspController::ApplyForce);
			IC->BindAction(NextGraspAction, IE_Pressed, this, &UMCGraspController::NextGrasp);
			IC->BindAction(PreviousGraspAction, IE_Pressed, this, &UMCGraspController::PreviousGrasp);
		}
	}
}

void UMCGraspController::ApplyForce(const float Input)
{
	GraspExecuter->UpdateGrasp(Input);
}

void UMCGraspController::NextGrasp()
{
	if (EquippedGrasps.Num() > 0)
	{
		if (CurrentGrasp >= EquippedGrasps.Num()-1)
		{
			CurrentGrasp = 0;
		}
		else
		{
			CurrentGrasp++;
		}

		GraspExecuter->SetGraspingData(UMCGraspAnimReader::ConvertAssetToStruct(EquippedGrasps[CurrentGrasp]));
	}
}

void UMCGraspController::PreviousGrasp()
{
	if (EquippedGrasps.Num() > 0)
	{
		if (CurrentGrasp <= 0)
		{
			CurrentGrasp = EquippedGrasps.Num() - 1;
		} else 
		{
			CurrentGrasp--;
		}

		GraspExecuter->SetGraspingData(UMCGraspAnimReader::ConvertAssetToStruct(EquippedGrasps[CurrentGrasp]));
	}
}