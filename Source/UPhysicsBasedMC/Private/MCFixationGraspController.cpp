// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCFixationGraspController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TagStatics.h"
#include "SLUtils.h"
#if ENGINE_MINOR_VERSION >= 19
#include "XRMotionControllerBase.h" // 4.19
#endif

// Constructor, set default values
UMCFixationGraspController::UMCFixationGraspController()
{
	InitSphereRadius(3.f);
	bGenerateOverlapEvents = true;
	bWeldFixation = true;
	ObjectMaxLength = 50.f;
	ObjectMaxMass = 15.f;
}

// Called when the game starts or when spawned
void UMCFixationGraspController::BeginPlay()
{
	Super::BeginPlay();

	// Get the semantic log runtime manager from the world
	for (TActorIterator<ASLRuntimeManager>RMItr(GetWorld()); RMItr; ++RMItr)
	{
		SemLogRuntimeManager = *RMItr;
		break;
	}
}

// Init fixation grasp	
void UMCFixationGraspController::Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC, UInputComponent* InIC)
{
	// Set pointer of skeletal hand
	SkeletalHand = InHand;

	// Set hand semantic logging (SL) individual name
	int32 TagIndex = FTagStatics::GetTagTypeIndex(InHand->ComponentTags, "SemLog");
	// If tag type exist, read the Class and the Id
	if (TagIndex != INDEX_NONE)
	{
		HandIndividual = FOwlIndividualName("log",
			FTagStatics::GetKeyValue(InHand->ComponentTags[TagIndex], "Class"),
			FTagStatics::GetKeyValue(InHand->ComponentTags[TagIndex], "Id"));
	}

	// Setup input
	if (InIC)
	{
		SetupInputBindings(InMC, InIC);
	}
	else
	{
		// Get the input controller for mapping the grasping control inputs
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			UInputComponent* IC = PC->InputComponent;
			if (IC)
			{
				SetupInputBindings(InMC, IC);
			}
		}
	}
	
	// Bind overlap events
	OnComponentBeginOverlap.AddDynamic(this, &UMCFixationGraspController::OnFixationGraspAreaBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UMCFixationGraspController::OnFixationGraspAreaEndOverlap);
}

// Setup input bindings
void UMCFixationGraspController::SetupInputBindings(UMotionControllerComponent* InMC, UInputComponent* InIC)
{
	// Check hand type
#if ENGINE_MINOR_VERSION >= 19
	if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
#else
	if(InMC->Hand == EControllerHand::Left)
#endif
	{
		InIC->BindAction("LeftFixate", IE_Pressed, this, &UMCFixationGraspController::TryToFixate);
		InIC->BindAction("LeftFixate", IE_Released, this, &UMCFixationGraspController::TryToDetach);
	}
#if ENGINE_MINOR_VERSION >= 19
	if (InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
#else
	if (InMC->Hand == EControllerHand::Right)
#endif
	{
		InIC->BindAction("RightFixate", IE_Pressed, this, &UMCFixationGraspController::TryToFixate);
		InIC->BindAction("RightFixate", IE_Released, this, &UMCFixationGraspController::TryToDetach);
	}

}

// Try to fixate object to hand
void UMCFixationGraspController::TryToFixate()
{
	while (!FixatedObject && ObjectsInReach.Num() > 0)
	{
		// Pop a SMA
		AStaticMeshActor* SMA = ObjectsInReach.Pop();

		// Check if the actor is graspable
		if (CanBeGrasped(SMA))
		{
			FixateObject(SMA);
		}		
	}
}

// Fixate object to hand
void UMCFixationGraspController::FixateObject(AStaticMeshActor* InSMA)
{
	// Disable physics and overlap events
	UStaticMeshComponent* SMC = InSMA->GetStaticMeshComponent();
	SMC->SetSimulatePhysics(false);
	//SMC->bGenerateOverlapEvents = false; // We want the object to continue to generate overlap events (e.g. semantic contacts)
	
	InSMA->AttachToComponent(SkeletalHand, FAttachmentTransformRules(
	EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, bWeldFixation));
	//SMC->AttachToComponent(SkeletalHand, FAttachmentTransformRules(
	//	EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, bWeldFixation));
	//InSMA->AttachToActor(SkeletalHand, FAttachmentTransformRules(
	//	EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, bWeldFixation));

	// Disable overlap checks during fixation grasp
	bGenerateOverlapEvents = false;

	// Set the fixated object
	FixatedObject = InSMA;

	// Clear objects in reach array
	ObjectsInReach.Empty();

	// Start grasp event
	UMCFixationGraspController::StartGraspEvent(FixatedObject);
}

// Detach fixation
void UMCFixationGraspController::TryToDetach()
{
	if (FixatedObject)
	{
		// Get current velocity before detachment (gets reseted)
		const FVector CurrVel = FixatedObject->GetVelocity();

		// Detach object from hand
		UStaticMeshComponent* SMC = FixatedObject->GetStaticMeshComponent();
		SMC->DetachFromComponent(FDetachmentTransformRules(
			EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));

		// Enable physics with and apply current hand velocity, clear pointer to object
		SMC->SetSimulatePhysics(true);
		SMC->bGenerateOverlapEvents = true;
		SMC->SetPhysicsLinearVelocity(CurrVel);
				
		// Enable and update overlaps
		bGenerateOverlapEvents = true;
		UpdateOverlaps();

		// Finish grasp event
		UMCFixationGraspController::FinishGraspEvent(FixatedObject);

		// Clear fixate object reference
		FixatedObject = nullptr;
	}
}

// Check if object is graspable
bool UMCFixationGraspController::CanBeGrasped(AStaticMeshActor* InSMA)
{
	// Check if the object is movable
	if (!InSMA->IsRootComponentMovable())
	{
		return false;
	}

	// Check if actor has a static mesh component
	if (UStaticMeshComponent* SMC = InSMA->GetStaticMeshComponent())
	{
		// Check if component has physics on
		if (!SMC->IsSimulatingPhysics())
		{
			return false;
		}

		// Check if object fits size
		if (SMC->GetMass() < ObjectMaxMass
			&& InSMA->GetComponentsBoundingBox().GetSize().Size() < ObjectMaxLength)
		{
			return true;
		}
	}
	return false;
}

// Function called when an item enters the fixation overlap area
void UMCFixationGraspController::OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (AStaticMeshActor* OtherSMA = Cast<AStaticMeshActor>(OtherActor))
	{
		ObjectsInReach.Emplace(OtherSMA);
	}

	//// TODO add separate functions for the force feedback
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//if (PC)
	//{
	//	FLatentActionInfo LatentActionInfo;
	//	LatentActionInfo.CallbackTarget = this;
	//	PC->PlayDynamicForceFeedback(1.0, 1.0, true, true, true, true, EDynamicForceFeedbackAction::Start, LatentActionInfo);
	//}
}

// Function called when an item leaves the fixation overlap area
void UMCFixationGraspController::OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Remove actor from array (if present)
	if (AStaticMeshActor* SMA = Cast<AStaticMeshActor>(OtherActor))
	{
		ObjectsInReach.Remove(SMA);
	}
}

// Start grasp event
bool UMCFixationGraspController::StartGraspEvent(AActor* OtherActor)
{
	// Check if actor has a semantic description
	int32 TagIndex = FTagStatics::GetTagTypeIndex(OtherActor->Tags, "SemLog");

	// If tag type exist, read the Class and the Id
	if (TagIndex != INDEX_NONE)
	{
		// Get the Class and Id from the semantic description
		const FString OtherActorClass = FTagStatics::GetKeyValue(OtherActor->Tags[TagIndex], "Class");
		const FString OtherActorId = FTagStatics::GetKeyValue(OtherActor->Tags[TagIndex], "Id");

		// Example of a contact event represented in OWL:
		/********************************************************************
		<!-- Event node described with a FOwlTriple (Subject-Predicate-Object) and Properties: -->
		<owl:NamedIndividual rdf:about="&log;GraspingSomething_S1dz">
		<!-- List of the event properties as FOwlTriple (Subject-Predicate-Object): -->
		<rdf:type rdf:resource="&knowrob;GraspingSomething"/>
		<knowrob:taskContext rdf:datatype="&xsd;string">Grasp-LeftHand_BRmZ-Bowl3_9w2Y</knowrob:taskContext>
		<knowrob:startTime rdf:resource="&log;timepoint_22.053652"/>
		<knowrob:objectActedOn rdf:resource="&log;Bowl3_9w2Y"/>
		<knowrob:performedBy rdf:resource="&log;LeftHand_BRmZ"/>
		<knowrob:endTime rdf:resource="&log;timepoint_32.28545"/>
		</owl:NamedIndividual>
		*********************************************************************/

		// Create contact event and other actor individual
		const FOwlIndividualName OtherIndividual("log", OtherActorClass, OtherActorId);
		const FOwlIndividualName GraspingIndividual("log", "GraspingSomething", FSLUtils::GenerateRandomFString(4));
		// Owl prefixed names
		const FOwlPrefixName RdfType("rdf", "type");
		const FOwlPrefixName RdfAbout("rdf", "about");
		const FOwlPrefixName RdfResource("rdf", "resource");
		const FOwlPrefixName RdfDatatype("rdf", "datatype");
		const FOwlPrefixName TaskContext("knowrob", "taskContext");
		const FOwlPrefixName PerformedBy("knowrob", "performedBy");
		const FOwlPrefixName ActedOn("knowrob", "objectActedOn");
		const FOwlPrefixName OwlNamedIndividual("owl", "NamedIndividual");
		// Owl classes
		const FOwlClass XsdString("xsd", "string");
		const FOwlClass GraspingSomething("knowrob", "GraspingSomething");

		// Add the event properties
		TArray <FOwlTriple> Properties;
		Properties.Add(FOwlTriple(RdfType, RdfResource, GraspingSomething));
		Properties.Add(FOwlTriple(TaskContext, RdfDatatype, XsdString,
			"Grasp-" + OtherIndividual.GetName() + "-" + HandIndividual.GetName()));
		Properties.Add(FOwlTriple(PerformedBy, RdfResource, HandIndividual));
		Properties.Add(FOwlTriple(ActedOn, RdfResource, OtherIndividual));

		// Create the contact event
		GraspEvent = MakeShareable(new FOwlNode(
			OwlNamedIndividual, RdfAbout, GraspingIndividual, Properties));

		// Start the event with the given properties
		return SemLogRuntimeManager->StartEvent(GraspEvent);
	}
	return false;
}

// Finish grasp event
bool UMCFixationGraspController::FinishGraspEvent(AActor* OtherActor)
{
	// Check if event started
	if (GraspEvent.IsValid())
	{
		return SemLogRuntimeManager->FinishEvent(GraspEvent);
		// Clear event
		GraspEvent.Reset();
	}
	return false;
}
