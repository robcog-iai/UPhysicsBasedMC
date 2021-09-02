// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspHelperController.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UMCGraspHelperController::UMCGraspHelperController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bIgnore = false;

	// Default physics properties
	SetGenerateOverlapEvents(true);

	// Default values
#if WITH_EDITORONLY_DATA
	HandType = EMCHandType::Left;
#endif // WITH_EDITORONLY_DATA

	bHelpIsActive = false;
	GraspedObject = nullptr;

	InputActionName = "LeftGraspHelper";

	bUseAttractionForce = false;
	AttractionForceFactor = 1000.f;

	bUsePID = false;

	// PID values (acc)
	LocControlType = EMCGraspHelp6DControlType::Acceleration;
	PLoc = 10;
	ILoc = 0;
	DLoc = 0;
	MaxLoc = 100;

	RotControlType = EMCGraspHelp6DControlType::Velocity;
	PRot = 1;
	IRot = 0;
	DRot = 0;
	MaxRot = 10;

	bUseAttachment = false;
	bUseConstraintComponent = false;
	ConstraintStiffness = 500.f;
	ConstraintDamping = 5.f;
	ConstraintContactDistance = 1.f;
	bConstraintParentDominates = false;
	BoneName = "lHand";

	bDisableGravity = true;

	bDecreaseMass = true;
	MassScaleValue = 0.1f;

	//ObjectWeightLimit = 15.0f;
	//ObjectVolumeLimit = 30000.0f; // 1000cm^3 = 1 Liter
}

// Called when the game starts
void UMCGraspHelperController::BeginPlay()
{
	Super::BeginPlay();

	if (bIgnore)
	{
		return;
	}
	Init();
}

// Called every frame, used for timeline visualizations, activated and deactivated on request
void UMCGraspHelperController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateHelp(DeltaTime);
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
			BoneName = "lHand";
		}
		else if (HandType == EMCHandType::Right)
		{
			InputActionName = "RightGraspHelper";
			BoneName = "rHand";
		}
	}
}
#endif // WITH_EDITOR

// Init 
void UMCGraspHelperController::Init()
{
	// Bind user input
	SetupInputBindings();

	if (ASkeletalMeshActor* AsSkelMA = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		OwnerSkelMC = AsSkelMA->GetSkeletalMeshComponent();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Owner is not a skeletal component.. aborting.."),
			*FString(__FUNCTION__), __LINE__);
		return;
	}

	if (bUseConstraintComponent)
	{
		ConstraintHelperComponent = NewObject<UPhysicsConstraintComponent>(this, FName("ConstraintHelperComponent"));
		ConstraintHelperComponent->RegisterComponent();
		ConstraintHelperComponent->AttachToComponent(OwnerSkelMC, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);


		ConstraintHelperComponent->ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 0.1f);
		ConstraintHelperComponent->ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 0.1f);
		ConstraintHelperComponent->ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, 0.1f);

		ConstraintHelperComponent->ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 0.1f);
		ConstraintHelperComponent->ConstraintInstance.SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 0.1f);
		ConstraintHelperComponent->ConstraintInstance.SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 0.1f);

		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.LinearLimit.bSoftConstraint = true;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.LinearLimit.Stiffness = ConstraintStiffness;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.LinearLimit.Damping = ConstraintDamping;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.LinearLimit.ContactDistance = ConstraintContactDistance;

		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.ConeLimit.bSoftConstraint = true;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.ConeLimit.Stiffness = ConstraintStiffness;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.ConeLimit.Damping = ConstraintDamping;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.ConeLimit.ContactDistance = ConstraintContactDistance;

		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.TwistLimit.bSoftConstraint = true;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.TwistLimit.Stiffness = ConstraintStiffness;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.TwistLimit.Damping = ConstraintDamping;
		ConstraintHelperComponent->ConstraintInstance.ProfileInstance.TwistLimit.ContactDistance = ConstraintContactDistance;

		if (bConstraintParentDominates)
		{
			ConstraintHelperComponent->ConstraintInstance.EnableParentDominates();
		}
	}

	// Bind overlap functions
	OnComponentBeginOverlap.AddDynamic(this, &UMCGraspHelperController::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UMCGraspHelperController::OnOverlapEnd);
}

// Bind user inputs
void UMCGraspHelperController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAction(InputActionName, IE_Pressed, this, &UMCGraspHelperController::ToggleHelp);
			//IC->BindAction(InputActionName, IE_Pressed, this, &UMCGraspHelperController::StartHelp);
			//IC->BindAction(InputActionName, IE_Released, this, &UMCGraspHelperController::StopHelp);
		}
	}
}

// Start helping with grasp
void UMCGraspHelperController::StartHelp()
{
	// Set the properties for the object to thelp
	if (SetGraspedObjectProperties())
	{
		if (bUseAttachment)
		{
			if (bUseConstraintComponent)
			{
				ConstraintHelperComponent->SetConstrainedComponents(OwnerSkelMC, BoneName, GraspedObjectSMC, NAME_None);
			}
			else
			{
				GraspedObjectSMC->SetSimulatePhysics(false);
				GraspedObject->AttachToComponent(OwnerSkelMC, FAttachmentTransformRules::KeepWorldTransform, BoneName);
			}
		}
		else if (bUsePID)
		{
			Controller6DPID.Init(this, GraspedObjectSMC, LocControlType,
				PLoc, ILoc, DLoc, MaxLoc, RotControlType, PRot, IRot, DRot, MaxRot,
				this->GetComponentTransform());
			SetComponentTickEnabled(true);
		}
		else if (bUseAttractionForce)
		{
			SetComponentTickEnabled(true);
		}
		bHelpIsActive = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Could not setup help object, this should not happen.."), *FString(__FUNCTION__), __LINE__);
	}
}

// Stop helping with grasp
void UMCGraspHelperController::StopHelp()
{	
	if (bUseAttachment)
	{
		if (bUseConstraintComponent)
		{
			ConstraintHelperComponent->BreakConstraint();
		}
		else
		{
			GraspedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			GraspedObjectSMC->SetSimulatePhysics(true);
		}
	}
	else if (bUsePID)
	{		
		Controller6DPID.Clear();
		SetComponentTickEnabled(false);
	}
	else if(bUseAttractionForce)
	{
		SetComponentTickEnabled(false);
	}
	ResetGraspedObjectProperties();
	bHelpIsActive = false;	
}

// Toggle help
void UMCGraspHelperController::ToggleHelp()
{	
	if (!bHelpIsActive)
	{
		StartHelp();
	}
	else
	{
		StopHelp();
	}
}

// Update the grasp
void UMCGraspHelperController::UpdateHelp(float DeltaTime)
{
	if (GraspedObjectSMC)
	{
		if (bUsePID)
		{
			Controller6DPID.UpdateController(DeltaTime);
		}
		else if (bUseAttractionForce)
		{
			FVector Out = GetComponentLocation() - GraspedObject->GetActorLocation();
			Out *= DeltaTime * AttractionForceFactor;
			GraspedObjectSMC->AddForce(Out, NAME_None, true);
		}
	}
}

// Setup object help properties
bool UMCGraspHelperController::SetGraspedObjectProperties()
{
	if (GraspedObject)
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Grasped object is already set, this should not happen.."), *FString(__FUNCTION__), __LINE__);
	}

	// Get the best candidate from the overlap pool
	GraspedObject = GetBestCandidate();
	if (GraspedObject && GraspedObject->IsValidLowLevel() && !GraspedObject->IsPendingKillOrUnreachable())
	{
		if (UStaticMeshComponent* SMC = GraspedObject->GetStaticMeshComponent())
		{
			GraspedObjectSMC = SMC;		

			if (bDisableGravity)
			{
				GraspedObjectSMC->SetEnableGravity(false);
			}

			if (bDecreaseMass)
			{
				GraspedObjectSMC->SetMassScale(NAME_None, MassScaleValue);
			}

			// Pause candidate searches until object is released
			SetGenerateOverlapEvents(false);
			OverlappingCandidates.Empty();

			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find a grasp (best)candidate, this should not happen.."), *FString(__FUNCTION__), __LINE__);
	}
	return false;
}

// Clear object help properties
bool UMCGraspHelperController::ResetGraspedObjectProperties()
{
	if (GraspedObject && GraspedObject->IsValidLowLevel() && !GraspedObject->IsPendingKillOrUnreachable())
	{
		if (GraspedObjectSMC)
		{
			if (bDisableGravity)
			{
				GraspedObjectSMC->SetEnableGravity(true);
			}

			if (bDecreaseMass)
			{
				GraspedObjectSMC->SetMassScale(NAME_None, 1.f);
			}

			GraspedObjectSMC = nullptr;			
		}

		GraspedObject = nullptr;

		// Start searching for candidates againg
		SetGenerateOverlapEvents(true);
		UpdateOverlaps();

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Object to help is not valid, this should not happen.."), *FString(__FUNCTION__), __LINE__);
	}
	return false;
}

// Check if the object can should be helped with grasping
bool UMCGraspHelperController::IsAGoodCandidate(AStaticMeshActor* InObject)
{
#if ENGINE_MINOR_VERSION > 23 || ENGINE_MAJOR_VERSION > 4
	TArray<UActorComponent*> Components;
	InObject->GetComponents(UShapeComponent::StaticClass(), Components);
	for (const auto C : Components)
	{
		if (C->GetName().StartsWith("SLContactMonitor"))
		{
			return true;
		}
	}
#else
	// Check if object has a contact area
	for (const auto& C : InObject->GetComponentsByClass(UShapeComponent::StaticClass()))
	{
		if (C->GetName().StartsWith("SLContactMonitor"))
		{
			return true;
		}
	}
#endif
	//// Check if the object is movable
	//if (!InObject->IsRootComponentMovable())
	//{
	//	return false;
	//}

	//// Check if actor has a valid static mesh component
	//if (UStaticMeshComponent* SMC = InObject->GetStaticMeshComponent())
	//{
	//	// Check if component has physics on
	//	if (!SMC->IsSimulatingPhysics())
	//	{
	//		return false;
	//	}

	//	// Check that object is not too heavy/large
	//	if (SMC->GetMass() < ObjectWeightLimit 
	//		&& InObject->GetComponentsBoundingBox().GetVolume() < ObjectVolumeLimit)
	//	{
	//		return true;
	//	}
	//}

	return false;
}

// Get the best candidate from the overlapp pool
AStaticMeshActor* UMCGraspHelperController::GetBestCandidate()
{
	// Return candidate closest to the controller center
	AStaticMeshActor* BestCandidate = nullptr;
	float DistSQ = BIG_NUMBER;
	for (const auto& Candidate : OverlappingCandidates)
	{
		float CurrDistSQ = FVector::DistSquared(Candidate->GetActorLocation(), GetComponentLocation());
		if (CurrDistSQ < DistSQ)
		{
			BestCandidate = Candidate;
			DistSQ = CurrDistSQ;
		}
	}
	return BestCandidate;
}

// Called on overlap begin events
void UMCGraspHelperController::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherComp->GetName().StartsWith("SLContactMonitor"))
	{
		if (AStaticMeshActor* OtherAsSMA = Cast<AStaticMeshActor>(OtherActor))
		{
			OverlappingCandidates.Emplace(OtherAsSMA);
			//UE_LOG(LogTemp, Warning, TEXT("%s::%d *_* Added %s to OverlappingObjects (Num=%d).."),
			//	*FString(__FUNCTION__), __LINE__, *OtherAsSMA->GetName(), OverlappingCandidates.Num());
		}
	}
	//if (AStaticMeshActor* OtherAsSMA = Cast<AStaticMeshActor>(OtherActor))
	//{
	//	if (IsAGoodCandidate(OtherAsSMA))
	//	{
	//		OverlappingCandidates.Emplace(OtherAsSMA);
	//		UE_LOG(LogTemp, Warning, TEXT("%s::%d *_* Added %s to OverlappingCandidates (Num=%d).."),
	//			*FString(__FUNCTION__), __LINE__, *OtherAsSMA->GetName());
	//	}
	//}
}

// Called on overlap end events
void UMCGraspHelperController::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherComp->GetName().StartsWith("SLContactMonitor"))
	{	
		if (AStaticMeshActor* OtherAsSMA = Cast<AStaticMeshActor>(OtherActor))
		{
			int32 NumEl = OverlappingCandidates.Remove(OtherAsSMA);
			//if (NumEl > 0)
			//{
			//	UE_LOG(LogTemp, Error, TEXT("%s::%d *_* Removed %s from OverlappingObjects (Num=%d).."),
			//		*FString(__FUNCTION__), __LINE__, *OtherAsSMA->GetName(), OverlappingCandidates.Num());
			//}
		}
	}
}