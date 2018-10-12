// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MC6DController.h"
#include "MC6DControllerOffset.h"
#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif // WITH_EDITORONLY_DATA

// Sets default values for this component's properties
UMC6DController::UMC6DController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Disable tick, it will be enabled after init
	PrimaryComponentTick.bStartWithTickEnabled = false;

#if WITH_EDITORONLY_DATA
	bDisplayDeviceModel = true;
	DisplayModelSource = FName("SteamVR");
	bHiddenInGame = true;
#endif // WITH_EDITORONLY_DATA

	// Default values
	bUseSkeletalMesh = true;
	bUseStaticMesh = false;

	bApplyToAllChildBodies = false;

	//ControlTypeLoc = EMCControlType::Acceleration;
	//ControlTypeRot = EMCControlType::Velocity;
	ControlTypeLoc = EMCControlType::Position;
	ControlTypeRot = EMCControlType::Position;

	PLoc = 300.0f;
	ILoc = 0.0f;
	DLoc = 50.0f;
	MaxLoc = 9000.f;

	PRot = 128.0f;
	IRot = 0.0f;
	DRot = 0.0f;
	MaxRot = 1500.f;
}

// Destructor
UMC6DController::~UMC6DController()
{
}

// Called when the game starts
void UMC6DController::BeginPlay()
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
		if (USkeletalMeshComponent* SkelMesh = SkeletalMeshActor->GetSkeletalMeshComponent())
		{
			SkelMesh->SetMobility(EComponentMobility::Movable);
			SkelMesh->SetSimulatePhysics(true);
			SkelMesh->SetEnableGravity(false);
			FTransform Offset;
			if (UMC6DControllerOffset* OffsetComp = Cast<UMC6DControllerOffset>(SkeletalMeshActor->GetComponentByClass(UMC6DControllerOffset::StaticClass())))
			{
				Offset = OffsetComp->GetComponentTransform();
			}
			UMC6DController::CreateControllers(SkelMesh, Offset);
			SetComponentTickEnabled(true);
		}
	}
	else if (bUseStaticMesh && StaticMeshActor)
	{
		if (UStaticMeshComponent* StaticMesh = StaticMeshActor->GetStaticMeshComponent())
		{
			StaticMesh->SetMobility(EComponentMobility::Movable);
			StaticMesh->SetSimulatePhysics(true);
			StaticMesh->SetEnableGravity(false);
			FTransform Offset;
			if (UMC6DControllerOffset* OffsetComp = Cast<UMC6DControllerOffset>(StaticMeshActor->GetComponentByClass(UMC6DControllerOffset::StaticClass())))
			{
				Offset = OffsetComp->GetComponentTransform();
			}
			UMC6DController::CreateControllers(StaticMesh, Offset);
			SetComponentTickEnabled(true);
		}
	}
}


#if WITH_EDITOR
// Called when a property is changed in the editor
void UMC6DController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Radio button style between bUseSkeletalMesh, bUseStaticMesh
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DController, bUseSkeletalMesh))
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
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DController, bUseStaticMesh))
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
}
#endif // WITH_EDITOR

// Called every frame
void UMC6DController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update owner mesh location / rotation
	LocationController->Update(DeltaTime);
	RotationController->Update(DeltaTime);

	ControllerCallbacks.Update(DeltaTime);
}

// Create location and rotation controllers for the skeletal mesh component
void UMC6DController::CreateControllers(USkeletalMeshComponent* SkeletalMeshComp, FTransform Offset)
{
	LocationController = NewObject<UMC3DLocationController>(this);
	RotationController = NewObject<UMC3DRotationController>(this);

	LocationController->Init(this, SkeletalMeshComp, Offset, ControlTypeLoc, PLoc, ILoc, DLoc, MaxLoc, bApplyToAllChildBodies);
	RotationController->Init(this, SkeletalMeshComp, Offset, ControlTypeRot, PRot, IRot, DRot, MaxRot, bApplyToAllChildBodies);
}

// Create location and rotation controllers for the static mesh component
void UMC6DController::CreateControllers(UStaticMeshComponent* StaticMeshComp, FTransform Offset)
{
	LocationController = NewObject<UMC3DLocationController>(this);
	RotationController = NewObject<UMC3DRotationController>(this);

	LocationController->Init(this, StaticMeshComp, Offset, ControlTypeLoc, PLoc, ILoc, DLoc, MaxLoc, bApplyToAllChildBodies);
	RotationController->Init(this, StaticMeshComp, Offset, ControlTypeRot, PRot, IRot, DRot, MaxRot, bApplyToAllChildBodies);
}