// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "UMC6DController.h"
#include "MotionControllerComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "MC6DControlType.h"
#include "MC6DController.h"
#include "MC6DTarget.generated.h"

/**
* Hand type
*/
UENUM()
enum class EMC6DHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};

#if WITH_EDITOR
UENUM()
enum class EMC6DTermSelection : uint8
{
	P						UMETA(DisplayName = "P"),
	I						UMETA(DisplayName = "I"),
	D						UMETA(DisplayName = "D"),
	Max						UMETA(DisplayName = "Max"),
	Delta					UMETA(DisplayName = "Delta"),
};

UENUM()
enum class EMC6DMovementTypeSelection : uint8
{
	Rot						UMETA(DisplayName = "Rot"),
	Loc						UMETA(DisplayName = "Loc"),
};
#endif // WITH_EDITOR


/**
 * 6D physics based movement applied to the skeletal or static mesh pointed to
 */
UCLASS(ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC 6D Target"), hidecategories = (Physics, Collision, Lighting))
class UMC6DCONTROLLER_API UMC6DTarget : public UMotionControllerComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UMC6DTarget();
	
	// Dtor
	~UMC6DTarget() = default;

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

#if WITH_EDITOR
private:
	// Input for changing the PID values on the fly
	void SetupInputBindings();
	void SwitchMovementType();
	void SwitchTerm();
	void SelectP();
	void SelectI();
	void SelectD();
	void SelectMax();
	void SelectDelta();
	void IncSelection();
	void DecSelection();
#endif // WITH_EDITOR

private:
#if WITH_EDITOR
	// Hand type, to point to the right XRMotionControllers
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	EMC6DHandType HandType;
#endif // WITH_EDITOR

	// Control a skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	bool bUseSkeletalMesh;
	
	// Apply movement control to all bones of the skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseSkeletalMesh"))
	bool bApplyToAllSkeletalBodies;

	// Skeletal mesh actor to control
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseSkeletalMesh"))
	ASkeletalMeshActor* SkeletalMeshActor;

	// Control a static mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	bool bUseStaticMesh;

	// Static mesh actor to control
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseStaticMesh"))
	AStaticMeshActor* StaticMeshActor;

	/* Control */
	// Control type (location and rotation)
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	EMC6DControlType ControlType;

	// Location PID controller values
	UPROPERTY(EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float PLoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float ILoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float DLoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float MaxLoc;

	// Rotation PID controller values
	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float PRot;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float IRot;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float DRot;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float MaxRot;

#if WITH_EDITOR
	// Change PID values at runtime 
	UPROPERTY(EditAnywhere, Category = "Movement Control|Dynamic")
	bool bDynamicController;

	// The delta to add or remove to the pid values
	UPROPERTY(EditAnywhere, Category = "Movement Control|Dynamic", meta = (editcondition = "bDynamicController"))
	float EditDelta;

	// Currently selected movement type for online editing
	EMC6DMovementTypeSelection ActiveMovementType;

	// Currently selected pid term for online editing
	EMC6DTermSelection ActiveTerm;

#endif // WITH_EDITOR

	// Update fallback function binding
	FMC6DController Controller;
};
