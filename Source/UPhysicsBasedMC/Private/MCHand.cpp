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
	SetGenerateOverlapEvents(true);
	
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

	// Initialize poseable mesh
	PoseableMesh = ObjectInitializer.CreateDefaultSubobject<UPoseableMeshComponent>(this, TEXT("PoseableMesh"));

	// Turn on replictation
	this->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void UMCHand::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default, enable it if Init() is called
	SetComponentTickEnabled(false);

	// Ticks last
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;

	// Set the skeletal mesh for the poseable mesh
	PoseableMesh->SetSkeletalMesh(this->SkeletalMesh);
	PoseableMesh->SetMobility(EComponentMobility::Movable);

	// Writes the names of all bones into a replicated array
	GetBoneNames(ReplicatedBoneNames);
	ReplicatedBoneTransforms.SetNum(ReplicatedBoneNames.Num(), true);
}

// Called every frame, used for motion control
void UMCHand::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the movement control of the hand
	MovementController->Update(DeltaTime);

#if WITH_MULTIPLAYER

	if (bIsServer)
	{
		SendPose();
	}
	else
	{
		ReceivePose();
	}
#endif
}

// Init hand with the motion controllers
void UMCHand::Init(UMotionControllerComponent* InMC)
{
	// Check hand type
	EControllerHand HandType = EControllerHand::AnyHand;
	if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
	{
		HandType = EControllerHand::Left;
	}
	else if(InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
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

// Sets up a number of variables to be replicated
// I use 2 replicated arrays instead of just one Map, because replciation currently doesn't work for maps
void UMCHand::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMCHand, ReplicatedBoneNames);
	DOREPLIFETIME(UMCHand, ReplicatedBoneTransforms);
	DOREPLIFETIME(UMCHand, HasAttached);
	DOREPLIFETIME(UMCHand, AttachedMesh);
	DOREPLIFETIME(UMCHand, AttachedTransform);
}

// Send data about current hand position and attached mesh
void UMCHand::SendPose()
{
	int i = 0;
	for (FName Name : ReplicatedBoneNames) {
		FQuat Quat = GetBoneQuaternion(Name, EBoneSpaces::WorldSpace);
		FVector Location = GetBoneLocation(Name, EBoneSpaces::WorldSpace);
		FVector Scale = FVector(1, 1, 1);
		ReplicatedBoneTransforms[i] = FTransform(Quat, Location, Scale);
		i++;
	}
	// Only when mesh has been attached
	if (FixationGraspController->HasAttached)
	{
		AttachedMesh = FixationGraspController->FixatedObject;
		AttachedTransform = AttachedMesh->GetTransform();
		HasAttached = true;
	}
}

// Apply data about hand position to the poseable mesh
void UMCHand::ReceivePose()
{
	int i = 0;
	for (FName Name : ReplicatedBoneNames) {
		PoseableMesh->SetBoneTransformByName(Name, ReplicatedBoneTransforms[i], EBoneSpaces::WorldSpace);
		i++;
	}
	// Only when mesh has been attached
	// Since this ticks last we overwrite any other replication
	if (HasAttached)
	{
		AttachedMesh->SetActorTransform(AttachedTransform);
	}
}