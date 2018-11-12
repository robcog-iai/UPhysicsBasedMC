// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
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
	~UMC6DTarget();

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
	// Hand type
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	EMC6DHandType HandType;

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

	// Update fallback function binding
	FMC6DController ControllerUpdateCallbacks;
};
