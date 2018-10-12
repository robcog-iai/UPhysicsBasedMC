// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "MCControlType.h"
#include "MC6DControllerCallbacks.h"
#include "MC6DController.generated.h"

// Forward declaration
class UMC6DControllerOffset;
class UMC3DLocationController;
class UMC3DRotationController;

/**
 * 6D physics based movement applied to the skeletal or static mesh pointed to
 */
UCLASS(ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC6DController"), hidecategories = (Physics, Collision, Lighting))
class UPHYSICSBASEDMC_API UMC6DController : public UMotionControllerComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UMC6DController();
	
	// Dtor
	~UMC6DController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Create location and rotation controllers for the skeletal mesh comp
	void CreateControllers(USkeletalMeshComponent* SkeletalMeshComp, FTransform Offset);

	// Create location and rotation controllers for the static mesh comp
	void CreateControllers(UStaticMeshComponent* StaticMeshComp, FTransform Offset);

	// Empty, default update function 
	void Update_NONE(float DeltaTime) {};

private:
//#if WITH_EDITORONLY_DATA
//	// Location and orientation visualization of the component
//	UPROPERTY()
//	class UArrowComponent* ArrowVis;
//#endif // WITH_EDITORONLY_DATA

	// Control a skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	bool bUseSkeletalMesh;

	// Skeletal mesh actor to control
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseSkeletalMesh"))
	ASkeletalMeshActor* SkeletalMeshActor;

	
	// Apply movement control to all bones of the skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseSkeletalMesh"))
	bool bApplyToAllChildBodies;

	// Control a static mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	bool bUseStaticMesh;

	// Static mesh actor to control
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseStaticMesh"))
	AStaticMeshActor* StaticMeshActor;

	/* Control */
	// Control type for the location
	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	EMCControlType ControlTypeLoc;

	// Location PID controller values
	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float PLoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float ILoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float DLoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float MaxLoc;

	// Control type for the rotation
	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation")
	EMCControlType ControlTypeRot;

	// Rotation PID controller values
	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation")
	float PRot;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation")
	float IRot;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation")
	float DRot;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation")
	float MaxRot;

	// Update fallback function binding
	FMC6DControllerCallbacks ControllerCallbacks;

	// Location controller
	UPROPERTY()
	UMC3DLocationController* LocationController;

	// Rotation controller
	UPROPERTY()
	UMC3DRotationController* RotationController;
};
