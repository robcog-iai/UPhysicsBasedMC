// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCFixationGraspController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "XRMotionControllerBase.h"
#if WITH_SEMLOG
#include "SLGraspTrigger.h"
#endif // WITH_SEMLOG


// Constructor, set default values
UMCFixationGraspController::UMCFixationGraspController()
{
	InitSphereRadius(3.f);
	SetGenerateOverlapEvents(true);
	bWeldFixation = true;
	ObjectMaxLength = 50.f;
	ObjectMaxMass = 15.f;
}

// Called when the game starts or when spawned
void UMCFixationGraspController::BeginPlay()
{
	Super::BeginPlay();

}

// Called when actor removed from game or game ended
void UMCFixationGraspController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

#if WITH_SEMLOG
	if (SLGraspTrigger)
	{
		SLGraspTrigger->Finish(GetWorld()->GetTimeSeconds());
	}
#endif //WITH_SEMLOG

}

// Init fixation grasp	
void UMCFixationGraspController::Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC, UInputComponent* InIC)
{
	// Set pointer of skeletal hand
	SkeletalHand = InHand;
#if WITH_SEMLOG
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
	// Create the semantic grasp trigger
	SLGraspTrigger = NewObject<USLGraspTrigger>(this);
	// Check if hand is semantically annotated
	bGraspTriggerInit = SLGraspTrigger->Init(InHand);
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d !!!! bGraspTriggerInit=%d"),
		TEXT(__FUNCTION__), __LINE__, bGraspTriggerInit);
#endif //WITH_SEMLOG

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
				UMCFixationGraspController::SetupInputBindings(InMC, IC);
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
	if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
	{
		InIC->BindAction("LeftFixate", IE_Pressed, this, &UMCFixationGraspController::TryToFixate);
		InIC->BindAction("LeftFixate", IE_Released, this, &UMCFixationGraspController::TryToDetach);
	}
	if (InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
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
		if (UMCFixationGraspController::CanBeGrasped(SMA))
		{
			UMCFixationGraspController::FixateObject(SMA);
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
	SetGenerateOverlapEvents(false);

	// Set the fixated object
	FixatedObject = InSMA;

	// Clear objects in reach array
	ObjectsInReach.Empty();

#if WITH_SEMLOG
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
	if (bGraspTriggerInit)
	{
		UE_LOG(LogTemp, Warning, TEXT(">> %s::%d BEGIN GRASP"), TEXT(__FUNCTION__), __LINE__);
		SLGraspTrigger->BeginGrasp(InSMA, GetWorld()->GetTimeSeconds());
	}	
#endif //WITH_SEMLOG
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
		SMC->SetGenerateOverlapEvents(true);
		SMC->SetPhysicsLinearVelocity(CurrVel);
				
		// Enable and update overlaps
		SetGenerateOverlapEvents(true);
		UpdateOverlaps();

#if WITH_SEMLOG
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
	if (bGraspTriggerInit)
	{
		UE_LOG(LogTemp, Warning, TEXT(">> %s::%d END GRASP"), TEXT(__FUNCTION__), __LINE__);
		SLGraspTrigger->EndGrasp(FixatedObject, GetWorld()->GetTimeSeconds());
	}
#endif //WITH_SEMLOG

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
