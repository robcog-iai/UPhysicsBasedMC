// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCHand.h"

// Sets default values
UMCHand::UMCHand(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Disable ticking in the editor
	bTickInEditor = false;

	// Default values for the skeletal mesh
	SetSimulatePhysics(true);
	SetEnableGravity(false);
	SetCollisionProfileName(TEXT("BlockAll"));
	bGenerateOverlapEvents = true;
	
	// Create the movement controller component
	MovementController = ObjectInitializer.CreateDefaultSubobject<UMCMovementController6D>(
		this, FName(*GetName().Append(TEXT("_MovementController"))));
	MovementController->SetupAttachment(this);

	// Create grasp component
	GraspController = ObjectInitializer.CreateDefaultSubobject<UMCGraspController>(
		this, FName(*GetName().Append(TEXT("_GraspController"))));

	// Create fixation grasp component
	FixationGraspController = ObjectInitializer.CreateDefaultSubobject<UMCFixationGraspController>(
		this, FName(*GetName().Append(TEXT("_FixationGraspController"))));
	FixationGraspController->SetupAttachment(this);

	// Enable fixation grasp by default
	bEnableFixationGrasp = true;
}

// Called when the game starts or when spawned
void UMCHand::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default, enable it if Init() is called
	SetComponentTickEnabled(false);
}

// Called every frame, used for motion control
void UMCHand::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the movement control of the hand
	MovementController->Update(DeltaTime);
}

// Init hand with the motion controllers
void UMCHand::Init(UMotionControllerComponent* InMC)
{
	// Check hand type
	EControllerHand HandType = EControllerHand::AnyHand;
#if ENGINE_MINOR_VERSION >= 19
	if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
#else
	if (InMC->Hand == EControllerHand::Left)
#endif
	{
		HandType = EControllerHand::Left;
	}
#if ENGINE_MINOR_VERSION >= 19
	else if(InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
#else
	else if(InMC->Hand == EControllerHand::Right)
#endif
	{
		HandType = EControllerHand::Right;
	}

	// Init the movement controller
	MovementController->Init(this, InMC);	

	// Init the grasp controller
	GraspController->Init(this, HandType);

	// Init the fixation grasp controller
	if (bEnableFixationGrasp)
	{
		FixationGraspController->Init(this, InMC);
	}
	else
	{
		FixationGraspController->DestroyComponent();
	}

	// Enable Tick
	SetComponentTickEnabled(true);
}

// Update default values if properties have been changed in the editor
#if WITH_EDITOR
void UMCHand::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	// Call the base class version  
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//// Get the name of the property that was changed  
	//FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	//// If hand type has been changed
	//if ((PropertyName == GET_MEMBER_NAME_CHECKED(UMCHand, HandType)))
	//{
	//	AMCHand::SetupHandDefaultValues(HandType);
	//}
}
#endif