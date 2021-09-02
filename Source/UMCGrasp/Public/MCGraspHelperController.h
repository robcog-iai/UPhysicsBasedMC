// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "UMCGrasp.h"
#include "Components/SphereComponent.h"
#include "MCStructs.h"
#include "MCGraspHelper6DPIDController.h"
#include "MCGraspHelperController.generated.h"

// Forward declaration
class AStaticMeshActor;
class UStaticMeshComponent;
class UPhysicsConstraintComponent;
class USkeletalMeshComponent;

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

	// Called every frame, used for timeline visualizations, activated and deactivated on request
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:
	// Init 
	void Init();

private:
	// Bind user inputs
	void SetupInputBindings();

	// Start helping with grasp
	void StartHelp();

	// Stop helping with grasp
	void StopHelp();

	// Toggle help
	void ToggleHelp();

	// Update the grasp
	void UpdateHelp(float DeltaTime);

	// Setup  object help properties
	bool SetGraspedObjectProperties();

	// Clear object help properties
	bool ResetGraspedObjectProperties();

	// Check if the object can should be helped with grasping
	bool IsAGoodCandidate(AStaticMeshActor* InObject);

	// Get the best candidate from the overlapp pool
	AStaticMeshActor* GetBestCandidate();

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

	// Input action name
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	FName InputActionName;

	// Attract bodies (meshes) on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|AttractionForce")
	bool bUseAttractionForce;

	// Force multiplicator
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|AttractionForce", meta = (editcondition = "bUseAttractionForce"))
	float AttractionForceFactor;

	// Maintain relative pose of the graped meshes
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID")
	bool bUsePID;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID")
	EMCGraspHelp6DControlType LocControlType;

	// Location PID controller values
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float PLoc;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float ILoc;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float DLoc;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float MaxLoc;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID")
	EMCGraspHelp6DControlType RotControlType;

	// Rot PID controller values
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float PRot;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float IRot;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float DRot;

	UPROPERTY(EditAnywhere, Category = "Grasp Helper|PID", meta = (editcondition = "bUsePID", ClampMin = 0))
	float MaxRot;

	// Rot/loc PID controller
	FMCGraspHelper6DPIDController Controller6DPID;

	// Attach bodies (meshes) on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment")
	bool bUseAttachment;

	// Weld bodies (meshes) on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment", meta = (editcondition = "bUseAttachment"))
	bool bUseConstraintComponent;

	// Constraint properties
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment", meta = (editcondition = "bUseConstraintComponent"))
	float ConstraintStiffness;

	// Constraint properties
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment", meta = (editcondition = "bUseConstraintComponent"))
	float ConstraintDamping;

	// Constraint properties
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment", meta = (editcondition = "bUseConstraintComponent"))
	float ConstraintContactDistance;

	// Constraint properties
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment", meta = (editcondition = "bUseConstraintComponent"))
	bool bConstraintParentDominates;

	// Weld bodies (meshes) on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper|Attachment", meta = (editcondition = "bUseAttachment"))
	FName BoneName;

	// Disable gravity on grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	bool bDisableGravity;

	// Decrease mass of object
	UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	bool bDecreaseMass;

	// Decrease mass of object
	UPROPERTY(EditAnywhere, Category = "Grasp Helper", meta = (editcondition = "bDecreaseMass"))
	float MassScaleValue;

	//// Weight (kg) limit for objects which should be helped for grasping
	//UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	//float ObjectWeightLimit;

	//// Volume (cm^3) limit for objects which should be helped for grasping (1000cm^3 = 1 Liter)
	//UPROPERTY(EditAnywhere, Category = "Grasp Helper")
	//float ObjectVolumeLimit;

	// Keep track if the help is active or not
	bool bHelpIsActive;

	// Pointer to the object which should be helped (nullptr if no object is in the area)
	AStaticMeshActor* GraspedObject;

	// The static mesh component of the object
	UStaticMeshComponent* GraspedObjectSMC;

	// Set of potential objects to be grasped (objects currently overlapping the area)
	TSet<AStaticMeshActor*> OverlappingCandidates;

	// Owner skeletal mesh component
	USkeletalMeshComponent* OwnerSkelMC;

	// Constraint component
	UPROPERTY(VisibleAnywhere, Category = "Grasp Helper")
	UPhysicsConstraintComponent* ConstraintHelperComponent;

};
