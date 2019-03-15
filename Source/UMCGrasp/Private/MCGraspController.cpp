// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspController.h"

// Sets default values for this component's properties
UMCGraspController::UMCGraspController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
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

void UMCGraspController::InitializeComponent()
{
	Super::InitializeComponent();


	if (ASkeletalMeshActor* ParentActor = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		//Create our grasping controllers for the left and right hand and register it
		GraspExecuter = NewObject<UMCGraspExecuter>(GetOwner(), UMCGraspExecuter::StaticClass(), TEXT("GraspExecuter"));
		GraspExecuter->RegisterComponent();
		if (EquippedGrasps.Num() > 0)
		{
			GraspExecuter->SetGraspingData(UMCRead::ConvertAssetToStruct(EquippedGrasps[CurrentGrasp]));
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

		GraspExecuter->SetGraspingData(UMCRead::ConvertAssetToStruct(EquippedGrasps[CurrentGrasp]));
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

		GraspExecuter->SetGraspingData(UMCRead::ConvertAssetToStruct(EquippedGrasps[CurrentGrasp]));
	}
}