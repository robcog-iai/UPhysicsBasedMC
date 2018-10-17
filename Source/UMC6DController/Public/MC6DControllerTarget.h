// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "MC6DControlType.h"
#include "MC6DControllerCallbacks.h"
#include "MC6DControllerTarget.generated.h"

/**
 * 6D physics based movement applied to the skeletal or static mesh pointed to
 */
UCLASS(ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC 6D Controller Target"), hidecategories = (Physics, Collision, Lighting))
class UMC6DCONTROLLER_API UMC6DControllerTarget : public UMotionControllerComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UMC6DControllerTarget();
	
	// Dtor
	~UMC6DControllerTarget();

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
	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float PLoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float ILoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float DLoc;

	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	float MaxLoc;

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
	FMC6DControllerCallbacks ControllerUpdateCallbacks;
};
