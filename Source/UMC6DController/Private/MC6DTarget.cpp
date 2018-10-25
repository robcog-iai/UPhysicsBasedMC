// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DTarget.h"
#include "MC6DOffset.h"
#if WITH_EDITOR
#include "XRMotionControllerBase.h"
#endif // WITH_EDITOR

// Sets default values for this component's properties
UMC6DTarget::UMC6DTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Disable tick, it will be enabled after init
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// Set ticking group 
	//PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;

#if WITH_EDITOR
	bDisplayDeviceModel = true;
	DisplayModelSource = FName("SteamVR");
	bHiddenInGame = true;
	MotionSource = FXRMotionControllerBase::LeftHandSourceId;
#endif // WITH_EDITOR

	// Default values
	HandType = EMC6DHandType::Left;
	bUseSkeletalMesh = true;
	bApplyToAllSkeletalBodies = false;
	bUseStaticMesh = false;

	//// Control type
	//ControlType = EMCControlType::Position;

	//// PID values (vel)
	//ControlType = EMCControlType::Velocity;
	//PLoc = 10.0f;
	//ILoc = 0.1f;
	//DLoc = 1.0f;
	//MaxLoc = 20.f;

	//PRot = 12.0f;
	//IRot = 0.1f;
	//DRot = 1.0f;
	//MaxRot = 15.f;

	// PID values (acc)
	ControlType = EMC6DControlType::Acceleration;
	PLoc = 750.0f;
	ILoc = 10.0f;
	DLoc = 75.0f;
	MaxLoc = 2500.f;

	PRot = 1500.0f;
	IRot = 10.0f;
	DRot = 400.0f;
	MaxRot = 5000.0f;
}

// Destructor
UMC6DTarget::~UMC6DTarget()
{
}

// Called when the game starts
void UMC6DTarget::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default
	// TODO / ISSUE why does this have to be manually disabled
	// even if PrimaryComponentTick.bStartWithTickEnabled = false;
	// this does not happen with USLVisManager.cpp
	SetComponentTickEnabled(false);

	// Check if owner has a valid static/skeletal mesh
	if (bUseSkeletalMesh && SkeletalMeshActor)
	{		
		if (USkeletalMeshComponent* SkelMeshComp = SkeletalMeshActor->GetSkeletalMeshComponent())
		{
			// Set mobility and physics parameters
			SkelMeshComp->SetMobility(EComponentMobility::Movable);
			SkelMeshComp->SetSimulatePhysics(true);
			SkelMeshComp->SetEnableGravity(false);

			// Initialize update callbacks with/without offset
			if (UMC6DOffset* OffsetComp = Cast<UMC6DOffset>(SkeletalMeshActor->GetComponentByClass(UMC6DOffset::StaticClass())))
			{
				ControllerUpdateCallbacks.Init(this, SkelMeshComp, bApplyToAllSkeletalBodies, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot,
					OffsetComp->GetComponentTransform());
			}
			else
			{
				ControllerUpdateCallbacks.Init(this, SkelMeshComp, bApplyToAllSkeletalBodies, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot);
			}
			// Enable Tick
			SetComponentTickEnabled(true);

			// Bind lambda function on the next tick to teleport the mesh to the target location
			// (cannot be done on begin play since the tracking is not active yet)
			// Timer delegate to be able to bind against non UObject functions
			FTimerDelegate TimerDelegateNextTick;
			TimerDelegateNextTick.BindLambda([&, SkelMeshComp]
			{
				// Reset velocity to 0 and teleport to the motion controller location
				// (the controller applies the PID output on the mesh before the lambda is called)
				SkelMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
				SkelMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
				SkelMeshComp->SetWorldTransform(GetComponentTransform(),
					false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			});
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegateNextTick);
		}
	}
	else if (bUseStaticMesh && StaticMeshActor)
	{
		if (UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent())
		{
			// Set mobility and physics parameters
			StaticMeshComp->SetMobility(EComponentMobility::Movable);
			StaticMeshComp->SetSimulatePhysics(true);
			StaticMeshComp->SetEnableGravity(false);

			// Initialize update callbacks with/without offset
			if (UMC6DOffset* OffsetComp = Cast<UMC6DOffset>(StaticMeshActor->GetComponentByClass(UMC6DOffset::StaticClass())))
			{
				ControllerUpdateCallbacks.Init(this, StaticMeshComp, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot,
					OffsetComp->GetComponentTransform());
			}
			else
			{
				ControllerUpdateCallbacks.Init(this, StaticMeshComp, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot);
			}
			// Enable Tick
			SetComponentTickEnabled(true);

			// Bind lambda function on the next tick to teleport the mesh to the target location
			// (cannot be done on begin play since the tracking is not active yet)
			// Timer delegate to be able to bind against non UObject functions
			FTimerDelegate TimerDelegateNextTick;
			TimerDelegateNextTick.BindLambda([&, StaticMeshComp]
			{
				// Reset velocity to 0 and teleport to the motion controller location
				// (the controller applies the PID output on the mesh before the lambda is called)
				StaticMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
				StaticMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
				StaticMeshComp->SetWorldTransform(GetComponentTransform(),
				 false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			});
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegateNextTick);
		}
	}
	// Could not set the update controller, tick remains disabled
}


#if WITH_EDITOR
// Called when a property is changed in the editor
void UMC6DTarget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Radio button style between bUseSkeletalMesh, bUseStaticMesh
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, bUseSkeletalMesh))
	{
		if (bUseSkeletalMesh)
		{
			bUseStaticMesh = false;
			if (StaticMeshActor)
			{
				StaticMeshActor = nullptr;
			}
		}
		else 
		{
			bUseStaticMesh = true;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, bUseStaticMesh))
	{
		if (bUseStaticMesh)
		{
			bUseSkeletalMesh = false;
			if (SkeletalMeshActor)
			{
				SkeletalMeshActor = nullptr;
			}
		}
		else
		{
			bUseSkeletalMesh = true;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, HandType))
	{
		if (HandType == EMC6DHandType::Left)
		{
			MotionSource = FXRMotionControllerBase::LeftHandSourceId;
		}
		else if (HandType == EMC6DHandType::Right)
		{
			MotionSource = FXRMotionControllerBase::RightHandSourceId;
		}
	}
}
#endif // WITH_EDITOR

// Called every frame
void UMC6DTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Apply target location to the referenced mesh
	ControllerUpdateCallbacks.Update(DeltaTime);
}
