// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCHand.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "TagStatics.h"
#include "SLUtils.h" // Semantic logger

// Sets default values
AMCHand::AMCHand()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Fixation grasp parameters	
	bFixationGraspEnabled = true;
	bTwoHandsFixationGraspEnabled = true;
	bMovementMimickingHand = false;
	bGraspHeld = false;
	bReadyForTwoHandsGrasp = false;
	OneHandFixationMaximumMass = 5.f;
	OneHandFixationMaximumLength = 50.f;
	TwoHandsFixationMaximumMass = 15.f;
	TwoHandsFixationMaximumLength = 120.f;

	// Set attachment collision component
	FixationGraspArea = CreateDefaultSubobject<USphereComponent>(TEXT("FixationGraspArea"));
	FixationGraspArea->SetupAttachment(GetRootComponent());
	FixationGraspArea->InitSphereRadius(3.f);

	// Set default as left hand
	HandType = EMCHandType::Left;

	// Set skeletal mesh default physics related values
	USkeletalMeshComponent* const SkelComp = GetSkeletalMeshComponent();
	SkelComp->SetSimulatePhysics(true);
	SkelComp->SetEnableGravity(false);
	SkelComp->SetCollisionProfileName(TEXT("BlockAll"));
	SkelComp->bGenerateOverlapEvents = true;

	// Angular drive default values
	AngularDriveMode = EAngularDriveMode::SLERP;
	Spring = 9000.0f;
	Damping = 1000.0f;
	ForceLimit = 0.0f;

	// Set fingers and their bone names default values
	AMCHand::SetupHandDefaultValues(HandType);

	// Set skeletal default values
	//AMCHand::SetupSkeletalDefaultValues(GetSkeletalMeshComponent());

}

// Called when the game starts or when spawned
void AMCHand::BeginPlay()
{
	Super::BeginPlay();

	// Setup player input bindings
	AMCHand::SetupInputBindings();

	// Get the semantic log runtime manager from the world
	for (TActorIterator<ASLRuntimeManager>RMItr(GetWorld()); RMItr; ++RMItr)
	{
		SemLogRuntimeManager = *RMItr;
		break;
	}

	// Disable tick as default
	SetActorTickEnabled(false);

	// Bind overlap events
	FixationGraspArea->OnComponentBeginOverlap.AddDynamic(this, &AMCHand::OnFixationGraspAreaBeginOverlap);
	FixationGraspArea->OnComponentEndOverlap.AddDynamic(this, &AMCHand::OnFixationGraspAreaEndOverlap);

	// Setup the values for controlling the hand fingers
	AMCHand::SetupAngularDriveValues(AngularDriveMode);

	// Set hand semantic logging (SL) individual name
	int32 TagIndex = FTagStatics::GetTagTypeIndex(Tags, "SemLog");
	// If tag type exist, read the Class and the Id
	if (TagIndex != INDEX_NONE)
	{
		HandIndividual = FOwlIndividualName("log",
			FTagStatics::GetKeyValue(Tags[TagIndex], "Class"),
			FTagStatics::GetKeyValue(Tags[TagIndex], "Id"));
	}
}

// Called every frame, used for motion control
void AMCHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// Two hand grasp hack
	//if (bMovementMimickingHand)
	//{
	//	//SetActorLocation(OtherHand->GetActorLocation() + MimickingRelativeLocation);
	//	//SetActorRotation(OtherHand->GetActorQuat() * MimickingRelativeRotation);
	//	//UE_LOG(LogTemp, Warning, TEXT("%s : is mimicking hand"), *GetName());

	//	//if (OtherHand)
	//	//{
	//	//	TArray<AActor*> AttachedActors;
	//	//	OtherHand->GetAttachedActors(AttachedActors);
	//	//	for (const auto& ActItr : AttachedActors)
	//	//	{
	//	//		UE_LOG(LogTemp, Warning, TEXT(" \t\t Attached actor: %s"), *ActItr->GetName());
	//	//	}
	//	//}

	//	//if (!AMCHand::IsTwoHandGraspStillValid())
	//	//{
	//	//	AMCHand::DetachFixationGrasp();
	//	//}
	//}
}

// Setup player input bindings
void AMCHand::SetupInputBindings()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PC)
	{
		UInputComponent* IC = PC->InputComponent;
		if (IC)
		{
			if (HandType == EMCHandType::Left)
			{
				// Hand control binding
				IC->BindAxis("GraspWithLeftHand", this, &AMCHand::UpdateGrasp);
				// Hand action binding
				IC->BindAction("AttachToLeftHand", IE_Pressed, this, &AMCHand::TryFixationGrasp);
				IC->BindAction("AttachToLeftHand", IE_Released, this, &AMCHand::TryFixationGraspDetach);
			}
			else if (HandType == EMCHandType::Right)
			{
				// Hand control binding
				IC->BindAxis("GraspWithRightHand", this, &AMCHand::UpdateGrasp);
				// Hand action binding
				IC->BindAction("AttachToRightHand", IE_Pressed, this, &AMCHand::TryFixationGrasp);
				IC->BindAction("AttachToRightHand", IE_Released, this, &AMCHand::TryFixationGraspDetach);
			}
			IC->BindAction("SwitchGrasp", IE_Pressed, this, &AMCHand::SwitchGrasp);			
		}
	}
}

// Update default values if properties have been changed in the editor
#if WITH_EDITOR
void AMCHand::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	// Call the base class version  
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the name of the property that was changed  
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// If hand type has been changed
	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AMCHand, HandType)))
	{
		AMCHand::SetupHandDefaultValues(HandType);
	}

	// If the skeletal mesh has been changed
	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AMCHand, GetSkeletalMeshComponent())))
	{
		//AMCHand::SetupSkeletalDefaultValues(GetSkeletalMeshComponent());
	}
}
#endif

// Check if the object in reach is one-, two-hand(s), or not graspable
void AMCHand::OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Check if object is graspable
	const uint8 GraspType = CheckObjectGraspableType(OtherActor);

	if (GraspType == ONE_HAND_GRASPABLE)
	{
		OneHandGraspableObjects.Emplace(Cast<AStaticMeshActor>(OtherActor));
	}
	else if (GraspType == TWO_HANDS_GRASPABLE)
	{
		TwoHandsGraspableObject = Cast<AStaticMeshActor>(OtherActor);
	}
}

// Object out or grasping reach, remove as possible grasp object
void AMCHand::OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If present, remove from the graspable objects
	OneHandGraspableObjects.Remove(Cast<AStaticMeshActor>(OtherActor));

	// If it is a two hands graspable object, clear pointer, reset flags
	if (TwoHandsGraspableObject)
	{
		bReadyForTwoHandsGrasp = false;
		TwoHandsGraspableObject = nullptr;
	}
}

// Update the grasp pose
void AMCHand::UpdateGrasp(const float Goal)
{
	if (!OneHandGraspedObject)
	{
		for (const auto& ConstrMapItr : Thumb.FingerPartToConstraint)
		{
			ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Goal * 100.f)));
		}
		for (const auto& ConstrMapItr : Index.FingerPartToConstraint)
		{
			ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Goal * 100.f)));
		}
		for (const auto& ConstrMapItr : Middle.FingerPartToConstraint)
		{
			ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Goal * 100.f)));
		}
		for (const auto& ConstrMapItr : Ring.FingerPartToConstraint)
		{
			ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Goal * 100.f)));
		}
		for (const auto& ConstrMapItr : Pinky.FingerPartToConstraint)
		{
			ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(0.f, 0.f, Goal * 100.f)));
		}
	}
	else if (!bGraspHeld)
	{
		AMCHand::MaintainFingerPositions();
	}
}

// Switch the grasp pose
void AMCHand::SwitchGrasp()
{
}

// Fixation grasp via attachment of the object to the hand
bool AMCHand::TryOneHandFixationGrasp()
{
	// If no current grasp is active and there is at least one graspable object
	if ((!OneHandGraspedObject) && (OneHandGraspableObjects.Num() > 0))
	{
		// Get the object to be grasped from the pool of objects
		OneHandGraspedObject = OneHandGraspableObjects.Pop();

		// TODO bug report, overlaps flicker when object is attached to hand, this prevents directly attaching objects from one hand to another
		//if (OneHandGraspedObject->GetAttachParentActor() && OneHandGraspedObject->GetAttachParentActor()->IsA(AMCHand::StaticClass()))
		//{
		//	// Detach from other hand
		//	AMCHand* OtherHand = Cast<AMCHand>(OneHandGraspedObject->GetAttachParentActor());
		//	OtherHand->TryDetachFixationGrasp();
		//}

		// Disable physics on the object and attach it to the hand
		OneHandGraspedObject->GetStaticMeshComponent()->SetSimulatePhysics(false);
		OneHandGraspedObject->GetStaticMeshComponent()->bGenerateOverlapEvents = false;

		/*OneHandGraspedObject->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(
		EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));*/
		OneHandGraspedObject->AttachToActor(this, FAttachmentTransformRules(
			EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

		// Disable overlap checks for the fixation grasp area during active grasping
		FixationGraspArea->bGenerateOverlapEvents = false;

		// Start grasp event
		AMCHand::StartGraspEvent(OneHandGraspedObject);

		// Successful grasp
		return true;
	}
	return false;
}

// Fixation grasp of two hands attachment
bool AMCHand::TryTwoHandsFixationGrasp()
{
	// This hand is ready to grasp the object as a two hand grasp
	if (OtherHand && TwoHandsGraspableObject)
	{
		bReadyForTwoHandsGrasp = true;
	}

	// Check if other hand is ready as well
	if (bReadyForTwoHandsGrasp && OtherHand->bReadyForTwoHandsGrasp)
	{
		// Check that both hands are in contact with the same object
		if (TwoHandsGraspableObject && OtherHand->GetTwoHandsGraspableObject())
		{
			// Set the grasped object, and clear the graspable one
			TwoHandsGraspedObject = TwoHandsGraspableObject;
			TwoHandsGraspableObject = nullptr;

			// Disable physics on the object and attach it to the hand
			TwoHandsGraspedObject->GetStaticMeshComponent()->SetSimulatePhysics(false);
			TwoHandsGraspedObject->GetStaticMeshComponent()->bGenerateOverlapEvents = false;

			TwoHandsGraspedObject->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(
				EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

			// Disable overlaps of the fixation grasp area during the active grasp
			FixationGraspArea->bGenerateOverlapEvents = false;

			// Start grasp event
			AMCHand::StartGraspEvent(TwoHandsGraspedObject);
			OtherHand->StartGraspEvent(TwoHandsGraspedObject);

			// Set other hands grasp as well
			OtherHand->TwoHandsFixationGraspFromOther();

			return true;
		}
	}
	return false;
}

// Fixation grasp of two hands attachment (triggered by other hand)
void AMCHand::TwoHandsFixationGraspFromOther()
{
	// Clear the pointer to the graspable object
	TwoHandsGraspableObject = nullptr;

	// This hand will only be mimicking the movements with the grasped object
	SetActorTickEnabled(true);
	bMovementMimickingHand = true;

	MimickingRelativeLocation = OtherHand->GetActorLocation() - GetActorLocation();
	MimickingRelativeRotation = OtherHand->GetActorQuat()*GetActorQuat();

	// Disable overlaps of the fixation grasp area during the active grasp
	FixationGraspArea->bGenerateOverlapEvents = false;
}

// Detach fixation grasp from hand(s)
bool AMCHand::DetachFixationGrasp()
{
	// Trigger released, reset two grasp ready flag
	bReadyForTwoHandsGrasp = false;

	// Re-enable overlaps for the fixation grasp area
	FixationGraspArea->bGenerateOverlapEvents = true;

	// Release grasp position
	bGraspHeld = false;

	if (OneHandGraspedObject)
	{
		// Finish grasp event
		AMCHand::FinishGraspEvent(OneHandGraspedObject);

		// Detach object from hand
		OneHandGraspedObject->GetStaticMeshComponent()->DetachFromComponent(FDetachmentTransformRules(
			EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));

		// Enable physics with and apply current hand velocity, clear pointer to object
		OneHandGraspedObject->GetStaticMeshComponent()->SetSimulatePhysics(true);
		OneHandGraspedObject->GetStaticMeshComponent()->bGenerateOverlapEvents = true;
		OneHandGraspedObject->GetStaticMeshComponent()->SetPhysicsLinearVelocity(GetVelocity());
		OneHandGraspedObject = nullptr;
		return true;
	}
	else if (TwoHandsGraspedObject && OtherHand)
	{
		// Finish grasp event
		AMCHand::FinishGraspEvent(TwoHandsGraspedObject);
		OtherHand->FinishGraspEvent(TwoHandsGraspedObject);

		// Detach object from hand
		TwoHandsGraspedObject->GetStaticMeshComponent()->DetachFromComponent(FDetachmentTransformRules(
			EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));

		// Enable physics with and apply current hand velocity, clear pointer to object
		TwoHandsGraspedObject->GetStaticMeshComponent()->SetSimulatePhysics(true);
		TwoHandsGraspedObject->GetStaticMeshComponent()->SetPhysicsLinearVelocity(GetVelocity());
		TwoHandsGraspedObject->GetStaticMeshComponent()->bGenerateOverlapEvents = true;
		TwoHandsGraspedObject = nullptr;

		// Trigger detachment on other hand as well
		OtherHand->DetachTwoHandFixationGraspFromOther();
		return true;
	}
	else if (bMovementMimickingHand && OtherHand)
	{
		// Disable mimicking hand
		SetActorTickEnabled(false);
		bMovementMimickingHand = false;

		// Trigger detachment on other hand as well
		OtherHand->DetachTwoHandFixationGraspFromOther();
		return true;
	}
	return false;
}

// Detach fixation grasp from hand (triggered by the other hand)
bool AMCHand::DetachTwoHandFixationGraspFromOther()
{
	// Re-enable overlaps for the fixation grasp area
	FixationGraspArea->bGenerateOverlapEvents = true;

	// Check grasp type of the hand (attachment or movement mimicking)
	if (TwoHandsGraspedObject)
	{
		// Detach object from hand
		TwoHandsGraspedObject->GetStaticMeshComponent()->DetachFromComponent(FDetachmentTransformRules(
			EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));

		// Enable physics with and apply current hand velocity, clear pointer to object
		TwoHandsGraspedObject->GetStaticMeshComponent()->SetSimulatePhysics(true);
		TwoHandsGraspedObject->GetStaticMeshComponent()->SetPhysicsLinearVelocity(GetVelocity());
		TwoHandsGraspedObject->GetStaticMeshComponent()->bGenerateOverlapEvents = true;
		TwoHandsGraspedObject = nullptr;
		return true;
	}
	else if (bMovementMimickingHand)
	{
		// Disable mimicking hand
		SetActorTickEnabled(false);
		bMovementMimickingHand = false;
		return true;
	}
	return false;
}

// Check if the two hand grasp is still valid (the hands have not moved away from each other)
bool AMCHand::IsTwoHandGraspStillValid()
{
	return true;
}

// Set pointer to other hand, used for two hand fixation grasp
void AMCHand::SetOtherHand(AMCHand* InOtherHand)
{
	OtherHand = InOtherHand;
}

// Start grasp event
bool AMCHand::StartGraspEvent(AActor* OtherActor)
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
bool AMCHand::FinishGraspEvent(AActor* OtherActor)
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

// Check how the object graspable
uint8 AMCHand::CheckObjectGraspableType(AActor* InActor)
{
	// Check if the static mesh actor can be grasped
	AStaticMeshActor* const SMActor = Cast<AStaticMeshActor>(InActor);
	if (SMActor)
	{
		// Check that actor is movable, has a static mesh component, and physics  enabled
		UStaticMeshComponent* const SMComp = SMActor->GetStaticMeshComponent();
		if (SMComp && SMActor->IsRootComponentMovable() && SMComp->IsSimulatingPhysics())
		{
			if (SMComp->GetMass() < OneHandFixationMaximumMass &&
				SMActor->GetComponentsBoundingBox().GetSize().Size() < OneHandFixationMaximumLength)
			{
				// one hand graspable size/dimension met
				return ONE_HAND_GRASPABLE;
			}
			else if (SMComp->GetMass() < TwoHandsFixationMaximumMass
				&& SMActor->GetComponentsBoundingBox().GetSize().Size() < TwoHandsFixationMaximumLength)
			{
				// two hand graspable size/dimensions met
				return TWO_HANDS_GRASPABLE;
			}
		}
	}
	// Actor cannot be attached
	return NOT_GRASPABLE;
}

// Hold grasp in the current position
void AMCHand::MaintainFingerPositions()
{
	//for (const auto& ConstrMapItr : Thumb.FingerPartToConstraint)
	//{
	//	ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(
	//		ConstrMapItr.Value->GetCurrentSwing2(),
	//		ConstrMapItr.Value->GetCurrentSwing1(),
	//		ConstrMapItr.Value->GetCurrentTwist())));
	//}
	//for (const auto& ConstrMapItr : Index.FingerPartToConstraint)
	//{
	//	ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(
	//		ConstrMapItr.Value->GetCurrentSwing2(),
	//		ConstrMapItr.Value->GetCurrentSwing1(),
	//		ConstrMapItr.Value->GetCurrentTwist())));
	//}
	//for (const auto& ConstrMapItr : Middle.FingerPartToConstraint)
	//{
	//	ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(
	//		ConstrMapItr.Value->GetCurrentSwing2(),
	//		ConstrMapItr.Value->GetCurrentSwing1(),
	//		ConstrMapItr.Value->GetCurrentTwist())));
	//}
	//for (const auto& ConstrMapItr : Ring.FingerPartToConstraint)
	//{
	//	ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(
	//		ConstrMapItr.Value->GetCurrentSwing2(),
	//		ConstrMapItr.Value->GetCurrentSwing1(),
	//		ConstrMapItr.Value->GetCurrentTwist())));
	//}
	//for (const auto& ConstrMapItr : Pinky.FingerPartToConstraint)
	//{
	//	ConstrMapItr.Value->SetAngularOrientationTarget(FQuat(FRotator(
	//		ConstrMapItr.Value->GetCurrentSwing2(),
	//		ConstrMapItr.Value->GetCurrentSwing1(),
	//		ConstrMapItr.Value->GetCurrentTwist())));
	//}

	bGraspHeld = true;
}

// Setup hand default values
void AMCHand::SetupHandDefaultValues(EMCHandType InHandType)
{
	if (InHandType == EMCHandType::Left)
	{
		Thumb.FingerType = EMCFingerType::Thumb;
		Thumb.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "thumb_01_l");
		Thumb.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "thumb_02_l");
		Thumb.FingerPartToBoneName.Add(EMCFingerPart::Distal, "thumb_03_l");

		Index.FingerType = EMCFingerType::Index;
		Index.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "index_01_l");
		Index.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "index_02_l");
		Index.FingerPartToBoneName.Add(EMCFingerPart::Distal, "index_03_l");

		Middle.FingerType = EMCFingerType::Middle;
		Middle.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "middle_01_l");
		Middle.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "middle_02_l");
		Middle.FingerPartToBoneName.Add(EMCFingerPart::Distal, "middle_03_l");

		Ring.FingerType = EMCFingerType::Ring;
		Ring.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "ring_01_l");
		Ring.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "ring_02_l");
		Ring.FingerPartToBoneName.Add(EMCFingerPart::Distal, "ring_03_l");

		Pinky.FingerType = EMCFingerType::Pinky;
		Pinky.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "pinky_01_l");
		Pinky.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "pinky_02_l");
		Pinky.FingerPartToBoneName.Add(EMCFingerPart::Distal, "pinky_03_l");
	}
	else if (InHandType == EMCHandType::Right)
	{
		Thumb.FingerType = EMCFingerType::Thumb;
		Thumb.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "thumb_01_r");
		Thumb.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "thumb_02_r");
		Thumb.FingerPartToBoneName.Add(EMCFingerPart::Distal, "thumb_03_r");

		Index.FingerType = EMCFingerType::Index;
		Index.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "index_01_r");
		Index.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "index_02_r");
		Index.FingerPartToBoneName.Add(EMCFingerPart::Distal, "index_03_r");

		Middle.FingerType = EMCFingerType::Middle;
		Middle.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "middle_01_r");
		Middle.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "middle_02_r");
		Middle.FingerPartToBoneName.Add(EMCFingerPart::Distal, "middle_03_r");

		Ring.FingerType = EMCFingerType::Ring;
		Ring.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "ring_01_r");
		Ring.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "ring_02_r");
		Ring.FingerPartToBoneName.Add(EMCFingerPart::Distal, "ring_03_r");

		Pinky.FingerType = EMCFingerType::Pinky;
		Pinky.FingerPartToBoneName.Add(EMCFingerPart::Proximal, "pinky_01_r");
		Pinky.FingerPartToBoneName.Add(EMCFingerPart::Intermediate, "pinky_02_r");
		Pinky.FingerPartToBoneName.Add(EMCFingerPart::Distal, "pinky_03_r");
	}
}

// Setup skeletal mesh default values
void AMCHand::SetupSkeletalDefaultValues(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	if (InSkeletalMeshComponent->GetPhysicsAsset())
	{
		// Hand joint velocity drive
		InSkeletalMeshComponent->SetAllMotorsAngularPositionDrive(true, true);

		// Set drive parameters
		InSkeletalMeshComponent->SetAllMotorsAngularDriveParams(Spring, Damping, ForceLimit);

		UE_LOG(LogTemp, Log, TEXT("AMCHand: SkeletalMeshComponent's angular motors set!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AMCHand: SkeletalMeshComponent's has no PhysicsAsset set!"));
	}
}

// Setup fingers angular drive values
void AMCHand::SetupAngularDriveValues(EAngularDriveMode::Type DriveMode)
{
	USkeletalMeshComponent* const SkelMeshComp = GetSkeletalMeshComponent();
	if (Thumb.SetFingerPartsConstraints(SkelMeshComp->Constraints))
	{
		Thumb.SetFingerDriveMode(DriveMode, Spring, Damping, ForceLimit);
	}
	if (Index.SetFingerPartsConstraints(SkelMeshComp->Constraints))
	{
		Index.SetFingerDriveMode(DriveMode, Spring, Damping, ForceLimit);
	}
	if (Middle.SetFingerPartsConstraints(SkelMeshComp->Constraints))
	{
		Middle.SetFingerDriveMode(DriveMode, Spring, Damping, ForceLimit);
	}
	if (Ring.SetFingerPartsConstraints(SkelMeshComp->Constraints))
	{
		Ring.SetFingerDriveMode(DriveMode, Spring, Damping, ForceLimit);
	}
	if (Pinky.SetFingerPartsConstraints(SkelMeshComp->Constraints))
	{
		Pinky.SetFingerDriveMode(DriveMode, Spring, Damping, ForceLimit);
	}
}

// Attach to left hand
void AMCHand::TryFixationGrasp()
{
	if (bFixationGraspEnabled)
	{
		// If one hand attachment is not possible, check for two hands
		if (AMCHand::TryOneHandFixationGrasp())
		{
			// If other hand is set and two hand grasp is enabled
			if (bTwoHandsFixationGraspEnabled)
			{
				// Try grasping with two hands
				AMCHand::TryTwoHandsFixationGrasp();
			}
		}
	}
}

// Attach to left hand
void AMCHand::TryFixationGraspDetach()
{
	if (bFixationGraspEnabled)
	{
		AMCHand::TryFixationGraspDetach();
	}
}