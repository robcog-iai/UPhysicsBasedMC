// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspHelperController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"

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

	GraspedObject = nullptr;

	InputActionName = "LeftGraspHelper";

	bWeldBodies = false;
	bDisableGravity = false;
	bDecreaseMass = false;

	DecreaseMassPercentage = 0.5f;
	DecreaseMassTo = -1.f;

	WeightLimit = 15.0f;
	VolumeLimit = 30000.0f; // 1000cm^3 = 1 Liter
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
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
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
		}
		else if (HandType == EMCHandType::Right)
		{
			InputActionName = "RightGraspHelper";
		}
	}
}
#endif // WITH_EDITOR

// Init 
void UMCGraspHelperController::Init()
{
	// Bind user input
	SetupInputBindings();

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

	if (IsComponentTickEnabled())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d Controller already helping: %s, this should not happen.."), *FString(__FUNCTION__), __LINE__, *GraspedObject->GetName());
	}

	// Set the properties for the object to thelp
	if (SetGraspedObject())
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d Could not setup help object, this should not happen.."), *FString(__FUNCTION__), __LINE__);
	}
}

// Stop helping with grasp
void UMCGraspHelperController::StopHelp()
{
	ClearHelpObject();
	SetComponentTickEnabled(false);
}

// Toggle help
void UMCGraspHelperController::ToggleHelp()
{	
	if (IsComponentTickEnabled())
	{
		StopHelp();
	}
	else
	{
		StartHelp();
	}
}

// Update the grasp
void UMCGraspHelperController::UpdateHelp(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("%s::%d Helping:%s;"), *FString(__FUNCTION__), __LINE__, *GraspedObject->GetName());
	FVector Out = GetComponentLocation() - GraspedObject->GetActorLocation();
	Out *= 5.f;
	GraspedObjectSMC->AddForce(Out, NAME_None, true);
}

// Setup object help properties
bool UMCGraspHelperController::SetGraspedObject()
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
			GraspedObjectSMC->SetEnableGravity(false);

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
bool UMCGraspHelperController::ClearHelpObject()
{
	if (GraspedObject && GraspedObject->IsValidLowLevel() && !GraspedObject->IsPendingKillOrUnreachable())
	{
		if (GraspedObjectSMC)
		{
			GraspedObjectSMC->SetEnableGravity(true);
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
	// Check if object has a contact area
	for (const auto& C : InObject->GetComponentsByClass(UShapeComponent::StaticClass()))
	{
		if (C->GetName().StartsWith("SLContactMonitor"))
		{
			return true;
		}
	}

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
	//	if (SMC->GetMass() < WeightLimit 
	//		&& InObject->GetComponentsBoundingBox().GetVolume() < VolumeLimit)
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
			UE_LOG(LogTemp, Warning, TEXT("%s::%d *_* Added %s to OverlappingObjects (Num=%d).."),
				*FString(__FUNCTION__), __LINE__, *OtherAsSMA->GetName(), OverlappingCandidates.Num());
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
			if (NumEl > 0)
			{
				UE_LOG(LogTemp, Error, TEXT("%s::%d *_* Removed %s from OverlappingObjects (Num=%d).."),
					*FString(__FUNCTION__), __LINE__, *OtherAsSMA->GetName(), OverlappingCandidates.Num());
			}
		}
	}
}