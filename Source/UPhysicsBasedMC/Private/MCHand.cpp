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
	//MovementController = CreateDefaultSubobject<UMCHandMovement>(TEXT("MovementController"));
	MovementController = ObjectInitializer.CreateDefaultSubobject<UMCHandMovement>(this, TEXT("MovementController"));

	//// Create grasp component
	////GraspController = CreateDefaultSubobject<UMCHandGrasp>(TEXT("GraspController"));
	//GraspController = ObjectInitializer.CreateDefaultSubobject<UMCHandGrasp>(this, TEXT("GraspController"));

	//// Create fixation grasp component
	////FixationGraspController = CreateDefaultSubobject<UMCFixationGrasp>(TEXT("FixationGraspController"));
	//FixationGraspController = ObjectInitializer.CreateDefaultSubobject<UMCFixationGrasp>(this, TEXT("FixationGraspController"));
	//FixationGraspController->SetupAttachment(this);

	//// Enable fixation grasp by default
	//bEnableFixationGrasp = true;
}

// Called when the game starts or when spawned
void UMCHand::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default, enable it in Init() (if called externally)
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
	// Init the movement controller
	MovementController->Init(this, InMC);	

	//// Init the grasp controller
	//GraspController->Init(this, InMC);

	//// Init the fixation grasp controller
	//if (bEnableFixationGrasp)
	//{
	//	FixationGraspController->Init(this, InMC);
	//}
	//else
	//{
	//	FixationGraspController->DestroyComponent();
	//}

	// Enable Tick
	SetComponentTickEnabled(true);
}