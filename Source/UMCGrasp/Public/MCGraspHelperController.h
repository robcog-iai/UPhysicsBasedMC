// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "UMCGrasp.h"
#include "Components/SphereComponent.h"
#include "MCStructs.h"
#include "MCGraspHelperController.generated.h"

// Forward declaration
class AStaticMeshActor;

/**
 * Grasp helper controller - applies various test forces to keep the graped objects in hand
 */
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Grasp Helper Controller"))
class UMCGRASP_API UMCGraspHelperController : public USphereComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCGraspHelperController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

private:
	// Bind user inputs
	void SetupInputBindings();

	// Start helping with grasp
	void StartHelp();

	// Stop helping with grasp
	void StopHelp();

	// Update the grasp
	void Update(float Value);

	// Check if the object can should be helped with grasping
	bool ShouldObjectBeHelped(AStaticMeshActor* InObject);

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
	// Skip initialization if true
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	bool bIgnore;

#if WITH_EDITORONLY_DATA
	// Hand type, to listen to the right inputs
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	EMCHandType HandType;
#endif // WITH_EDITORONLY_DATA

	// Input axis name
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	FName InputActionName;

	// Weld bodies (meshes) on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	bool bWeldBodies;

	// Disable gravity on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	bool bDisableGravity;

	// Decrease mass of object
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	bool bDecreaseMass;

	// Decrease mass of object
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	float DecreaseMassPercentage;

	// Decrease mass of object to
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	float DecreaseMassTo;

	// Weight (kg) limit that can be grasped
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	float WeightLimit;

	// Volume (cm^3) limit that can be grasped (1000cm^3 = 1 Liter)
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	float VolumeLimit;

	// Pointer to the grasped component (nullptr if nothing is grasped)
	AStaticMeshActor* GraspedObject;
};
