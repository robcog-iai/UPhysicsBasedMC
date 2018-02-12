// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCFixationGrasp.h"
#include "Kismet/GameplayStatics.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::Left/RightHandSourceId and GetHandEnumForSourceName()

// Constructor, set default values
UMCFixationGrasp::UMCFixationGrasp()
{
	InitSphereRadius(3.f);
}

// Called when the game starts or when spawned
void UMCFixationGrasp::BeginPlay()
{
	Super::BeginPlay();
}

// Init fixation grasp	
void UMCFixationGrasp::Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC)
{
	// Bind user input
	SetupInputBindings(InMC);

	// Bind overlap events
	OnComponentBeginOverlap.AddDynamic(this, &UMCFixationGrasp::OnFixationGraspAreaBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UMCFixationGrasp::OnFixationGraspAreaEndOverlap);
}

// Setup input bindings
void UMCFixationGrasp::SetupInputBindings(UMotionControllerComponent* InMC)
{
	// Get the input controller for mapping the grasping control inputs
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		UInputComponent* IC = PC->InputComponent;
		if (IC)
		{
			// Check hand type
			if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
			{
				IC->BindAction("LeftFixation", IE_Pressed, this, &UMCFixationGrasp::TryFixation);
				IC->BindAction("LeftFixation", IE_Released, this, &UMCFixationGrasp::DetachFixation);
			}
			else if (InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
			{
				IC->BindAction("RightFixation", IE_Pressed, this, &UMCFixationGrasp::TryFixation);
				IC->BindAction("RightFixation", IE_Released, this, &UMCFixationGrasp::DetachFixation);
			}
		}
	}
}

// Try to fixate object to hand
void UMCFixationGrasp::TryFixation()
{
	//// If no current grasp is active and there is at least one graspable object
	//if ((!OneHandGraspedObject) && (OneHandGraspableObjects.Num() > 0))
	//{
	//	// Get the object to be grasped from the pool of objects
	//	OneHandGraspedObject = OneHandGraspableObjects.Pop();

	//	// TODO bug report, overlaps flicker when object is attached to hand, this prevents directly attaching objects from one hand to another
	//	//if (OneHandGraspedObject->GetAttachParentActor() && OneHandGraspedObject->GetAttachParentActor()->IsA(AMCHand::StaticClass()))
	//	//{
	//	//	// Detach from other hand
	//	//	AMCHand* OtherHand = Cast<AMCHand>(OneHandGraspedObject->GetAttachParentActor());
	//	//	OtherHand->TryDetachFixationGrasp();
	//	//}

	//	// Disable physics on the object and attach it to the hand
	//	OneHandGraspedObject->GetStaticMeshComponent()->SetSimulatePhysics(false);
	//	OneHandGraspedObject->GetStaticMeshComponent()->bGenerateOverlapEvents = false;

	//	/*OneHandGraspedObject->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(
	//	EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));*/
	//	OneHandGraspedObject->AttachToActor(this, FAttachmentTransformRules(
	//		EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

	//	// Disable overlap checks for the fixation grasp area during active grasping
	//	FixationGraspArea->bGenerateOverlapEvents = false;

	//	// Start grasp event
	//	AMCHand::StartGraspEvent(OneHandGraspedObject);

	//	// Successful grasp
	//	return true;
	//}
	//return false;
}

// Detach fixation
void UMCFixationGrasp::DetachFixation()
{

}

// Function called when an item enters the fixation overlap area
void UMCFixationGrasp::OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//// Check if object is graspable
	//const uint8 GraspType = CheckObjectGraspableType(OtherActor);

	//if (GraspType == ONE_HAND_GRASPABLE)
	//{
	//	OneHandGraspableObjects.Emplace(Cast<AStaticMeshActor>(OtherActor));
	//}
	//else if (GraspType == TWO_HANDS_GRASPABLE)
	//{
	//	TwoHandsGraspableObject = Cast<AStaticMeshActor>(OtherActor);
	//}


}

// Function called when an item leaves the fixation overlap area
void UMCFixationGrasp::OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//// If present, remove from the graspable objects
	//OneHandGraspableObjects.Remove(Cast<AStaticMeshActor>(OtherActor));

	//// If it is a two hands graspable object, clear pointer, reset flags
	//if (TwoHandsGraspableObject)
	//{
	//	bReadyForTwoHandsGrasp = false;
	//	TwoHandsGraspableObject = nullptr;
	//}
}

