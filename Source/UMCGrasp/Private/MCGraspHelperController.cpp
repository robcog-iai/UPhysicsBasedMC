// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspHelperController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"

// Sets default values for this component's properties
UMCGraspHelperController::UMCGraspHelperController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bIgnore = false;

	// Default physics properties
	SetGenerateOverlapEvents(true);

	// Default values
#if WITH_EDITORONLY_DATA
	HandType = EMCHandType::Left;
#endif // WITH_EDITORONLY_DATA

	InputActionName = "LeftGraspHelper";

	bWeldBodies = false;
	bDisableGravity = false;
	bDecreaseMass = false;

	DecreaseMassPercentage = 0.5f;
	DecreaseMassTo = -1.f;

	WeightLimit = 15.0f;
	VolumeLimit = 30000.0f; // 1000cm^3 = 1 Liter
}

// Called when the game starts
void UMCGraspHelperController::BeginPlay()
{
	Super::BeginPlay();

	if (bIgnore)
	{
		return;
	}

	// Bind user input
	SetupInputBindings();

	// Bind overlap functions
	OnComponentBeginOverlap.AddDynamic(this, &UMCGraspHelperController::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UMCGraspHelperController::OnOverlapEnd);
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCGraspHelperController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCGraspHelperController, HandType))
	{
		if (HandType == EMCHandType::Left)
		{
			InputActionName = "LeftGraspHelper";
		}
		else if (HandType == EMCHandType::Right)
		{
			InputActionName = "RightGraspHelper";
		}
	}
}
#endif // WITH_EDITOR

// Bind user inputs
void UMCGraspHelperController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAction(InputActionName, IE_Pressed, this, &UMCGraspHelperController::StartHelp);
			IC->BindAction(InputActionName, IE_Released, this, &UMCGraspHelperController::StopHelp);
		}
	}
}

// Start helping with grasp
void UMCGraspHelperController::StartHelp()
{

}

// Stop helping with grasp
void UMCGraspHelperController::StopHelp()
{

}

// Update the grasp
void UMCGraspHelperController::Update(float Value)
{

}

// Check if the object can should be helped with grasping
bool UMCGraspHelperController::ShouldObjectBeHelped(AStaticMeshActor* InObject)
{
	// Check if the object is movable
	if (!InObject->IsRootComponentMovable())
	{
		return false;
	}

	// Check if actor has a valid static mesh component
	if (UStaticMeshComponent* SMC = InObject->GetStaticMeshComponent())
	{
		// Check if component has physics on
		if (!SMC->IsSimulatingPhysics())
		{
			return false;
		}

		// Check that object is not too heavy/large
		if (SMC->GetMass() < WeightLimit &&
			InObject->GetComponentsBoundingBox().GetVolume() < VolumeLimit)
		{
			return true;
		}
	}

	return false;
}

// Called on overlap begin events
void UMCGraspHelperController::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AStaticMeshActor* OtherAsSMA = Cast<AStaticMeshActor>(OtherActor))
	{
		
	}
}

// Called on overlap end events
void UMCGraspHelperController::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

}