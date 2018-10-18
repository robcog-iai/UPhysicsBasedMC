// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCFixationGrasp.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

// Default constructor
UMCFixationGrasp::UMCFixationGrasp()
{	
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Default sphere radius
	InitSphereRadius(3.0f);

	// Default physics properties
	SetGenerateOverlapEvents(true);

	// Default values
	InputActionName = "LeftFixate";
	bWeldBodies = true;
	WeightLimit = 15.0f;
	VolumeLimit = 30.0f;
}

// Called when the game starts
void UMCFixationGrasp::BeginPlay()
{
	Super::BeginPlay();

	// Check that parent (owner) is a skeletal or static mesh actor
	if (ASkeletalMeshActor* OwnerAsSkelMA = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		ParentAsSkelMA = OwnerAsSkelMA;
		UE_LOG(LogTemp, Warning, TEXT(">> %s::%d SK Name=%s"), TEXT(__FUNCTION__), __LINE__, *ParentAsSkelMA->GetName());
		UMCFixationGrasp::Init();
	}
	else if (AStaticMeshActor* OwnerAsSMA = Cast<AStaticMeshActor>(GetOwner()))
	{
		ParentAsSMA = OwnerAsSMA;
		UE_LOG(LogTemp, Warning, TEXT(">> %s::%d SM Name=%s"), TEXT(__FUNCTION__), __LINE__, *ParentAsSMA->GetName());
		UMCFixationGrasp::Init();
	}
}

// Init controller
void UMCFixationGrasp::Init()
{
	OnComponentBeginOverlap.AddDynamic(this, &UMCFixationGrasp::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UMCFixationGrasp::OnOverlapEnd);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			UMCFixationGrasp::SetupInputBindings(IC);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Bind user inputs
void UMCFixationGrasp::SetupInputBindings(UInputComponent* InIC)
{
	InIC->BindAction(InputActionName, IE_Pressed, this, &UMCFixationGrasp::FixateGrasp);
	InIC->BindAction(InputActionName, IE_Released, this, &UMCFixationGrasp::ReleaseGrasp);
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Try to fixate overlapping object to parent
void UMCFixationGrasp::FixateGrasp()
{
	// Check if it already grasping something
	if (GraspedActor)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}


// Free fixated object from parent
void UMCFixationGrasp::ReleaseGrasp()
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Called on overlap begin events
void UMCFixationGrasp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AStaticMeshActor* OtherAsSMA = Cast<AStaticMeshActor>(OtherActor))
	{
		ObjectsInSphereArea.Emplace(OtherAsSMA);
	}

	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}

// Called on overlap end events
void UMCFixationGrasp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// Remove actor from array (if present)
	if (AStaticMeshActor* OtherAsSMA = Cast<AStaticMeshActor>(OtherActor))
	{
		ObjectsInSphereArea.Remove(OtherAsSMA);
	}

	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}