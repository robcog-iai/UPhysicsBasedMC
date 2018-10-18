// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MCFixationGrasp.generated.h"

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
	// Init controller
	void Init();

	// Bind user inputs
	void SetupInputBindings(UInputComponent* InIC);

	// Try to fixate overlapping object to parent
	void FixateGrasp();

	// Free fixated object from parent
	void ReleaseGrasp();

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

private:
	// Weld bodies (meshes) on fixation
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	bool bWeldBodies;

	// Weight (kg) limit that can be grasped (fixated)
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	float WeightLimit;
	
	// Volume (liter) limit that can be grasped (fixated)
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	float VolumeLimit;

	// Input action name
	UPROPERTY(EditAnywhere, Category = "Fixation Grasp")
	FName InputActionName;

	// Pointer to the parent as a skeletal mesh actor
	class ASkeletalMeshActor* ParentAsSkelMA;

	// Pointer to the parent as a static mesh actor
	class AStaticMeshActor* ParentAsSMA;

	// Pointer to the grasped component (nullptr if nothing is grasped)
	class AStaticMeshActor* GraspedActor;

	// Potential objects that can be grasped, currently overlapping the sphere
	TArray<class AStaticMeshActor*> ObjectsInSphereArea;

};
