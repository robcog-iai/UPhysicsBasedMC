// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCParallelGripper.h"


// Sets default values for this component's properties
UMCParallelGripper::UMCParallelGripper()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Disable tick, it will be enabled after init
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Create the constraints
	LeftFingerConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftFingerConstraint"));	
	RightFingerConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RightFingerConstraint"));

	// Init constraints
	UMCParallelGripper::InitConstraint(LeftFingerConstraint);
	UMCParallelGripper::InitConstraint(RightFingerConstraint);
	
	// Set as children
	LeftFingerConstraint->SetupAttachment(this);
	RightFingerConstraint->SetupAttachment(this);
}


// Called when the game starts
void UMCParallelGripper::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);

	AStaticMeshActor* OwnerAsStaticMeshActor = Cast<AStaticMeshActor>(GetOwner());
	if (OwnerAsStaticMeshActor && LeftFinger && RightFinger)
	{
		// Make sure the constraints are set
		if (LeftFingerConstraint->ConstraintActor1 != OwnerAsStaticMeshActor)
		{
			LeftFingerConstraint->ConstraintActor1 = OwnerAsStaticMeshActor;
		}
		if (LeftFingerConstraint->ConstraintActor2 != LeftFinger)
		{
			LeftFingerConstraint->ConstraintActor2 = LeftFinger;
		}
		if (RightFingerConstraint->ConstraintActor1 != OwnerAsStaticMeshActor)
		{
			RightFingerConstraint->ConstraintActor1 = OwnerAsStaticMeshActor;
		}
		if (RightFingerConstraint->ConstraintActor2 != RightFinger)
		{
			RightFingerConstraint->ConstraintActor2 = RightFinger;
		}

		// Set fingers mobility and physics parameters
		UStaticMeshComponent* LeftSM = LeftFinger->GetStaticMeshComponent();
		UStaticMeshComponent* RightSM = RightFinger->GetStaticMeshComponent();
		UStaticMeshComponent* OwnerSM = OwnerAsStaticMeshActor->GetStaticMeshComponent();
		if (OwnerSM && LeftSM && RightSM)
		{
			OwnerSM->SetMobility(EComponentMobility::Movable);
			OwnerSM->SetSimulatePhysics(true);
			OwnerSM->SetEnableGravity(false);

			LeftSM->SetMobility(EComponentMobility::Movable);
			LeftSM->SetSimulatePhysics(true);
			LeftSM->SetEnableGravity(false);

			RightSM->SetMobility(EComponentMobility::Movable);
			RightSM->SetSimulatePhysics(true);
			RightSM->SetEnableGravity(false);

			UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
			//SetComponentTickEnabled(true);
		}
	}	
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCParallelGripper::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCParallelGripper, LeftFinger))
	{
		if (LeftFinger)
		{
			if (AStaticMeshActor* OwnerAsStaticMeshActor = Cast<AStaticMeshActor>(GetOwner()))
			{
				LeftFingerConstraint->ConstraintActor1 = OwnerAsStaticMeshActor;
			}
			LeftFingerConstraint->ConstraintActor2 = LeftFinger;
			LeftFingerConstraint->SetWorldTransform(LeftFinger->GetTransform());
		}
		else
		{
			LeftFingerConstraint->ConstraintActor1 = nullptr;
			LeftFingerConstraint->ConstraintActor2 = nullptr;
			LeftFingerConstraint->SetWorldTransform(GetOwner()->GetTransform());
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCParallelGripper, RightFinger))
	{
		if (RightFinger)
		{
			if (AStaticMeshActor* OwnerAsStaticMeshActor = Cast<AStaticMeshActor>(GetOwner()))
			{
				RightFingerConstraint->ConstraintActor1 = OwnerAsStaticMeshActor;
			}
			RightFingerConstraint->ConstraintActor2 = RightFinger;
			RightFingerConstraint->SetWorldTransform(RightFinger->GetTransform());
		}
		else
		{
			RightFingerConstraint->ConstraintActor1 = nullptr;
			RightFingerConstraint->ConstraintActor2 = nullptr;
			RightFingerConstraint->SetWorldTransform(GetOwner()->GetTransform());
		}
	}
}
#endif // WITH_EDITOR


// Called every frame
void UMCParallelGripper::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Set default values to the constraints
void UMCParallelGripper::InitConstraint(UPhysicsConstraintComponent* Constraint)
{
	Constraint->SetAngularSwing1Limit(ACM_Locked, 0.f);
	Constraint->SetAngularSwing2Limit(ACM_Locked, 0.f);
	Constraint->SetAngularTwistLimit(ACM_Locked, 0.f);
	Constraint->SetLinearXLimit(LCM_Limited, 3.5f);
	Constraint->SetLinearYLimit(LCM_Locked, 0.f);
	Constraint->SetLinearZLimit(LCM_Locked, 0.f);
}