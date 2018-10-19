// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MCFixationGrasp.generated.h"

// Forward declaration
class AStaticMeshActor;
class ASkeletalMeshActor;

/** Notify when an object is grasped */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FMCGraspBegin, uint32 /*MyId*/, uint32 /*OtherId*/, float /*Time*/);

/** Notify when an object is released */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FMCGraspEnd, uint32 /*MyId*/, uint32 /*OtherId*/, float /*Time*/);

/**
 * Sphere area that fixates objects to the owner according to some rules
 */
UCLASS(ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Fixation Grasp"))
class UMCGRASP_API UMCFixationGrasp : public USphereComponent
{
	GENERATED_BODY()
	
public:
	// Constructor
	UMCFixationGrasp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Bind user inputs
	void SetupInputBindings(UInputComponent* InIC);

	// Try to grasp (fixate) the overlapping object to parent
	void Grasp();

	// Release the grasp (fixation)
	void Release();

	// Check if the object can be grasped (not too heavy/large)
	bool CanObjectBeGrasped(AStaticMeshActor* InObject);

	// Fixate the given object to parent
	bool Fixate(AStaticMeshActor* InObject);

	// Event called when something starts to overlaps this component
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// Event called when something stops overlapping this component 
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	// Event called when grasp occurs
	FMCGraspBegin OnGraspBegin;

	// Event called when grasp ends
	FMCGraspEnd OnGraspEnd;

private:
	// Weld bodies (meshes) on fixation
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	bool bWeldBodies;

	// Weight (kg) limit that can be grasped
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	float WeightLimit;
	
	// Volume (cm^3) limit that can be grasped (1000cm^3 = 1 Liter)
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	float VolumeLimit;

	// Input action name
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	FName InputActionName;

	// Pointer to the grasped component (nullptr if nothing is grasped)
	AStaticMeshActor* GraspedObject;

	// Potential objects that can be grasped, currently overlapping the sphere
	TArray<AStaticMeshActor*> ObjectsInSphereArea;

};
