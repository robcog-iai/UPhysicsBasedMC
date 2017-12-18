// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/SkeletalMeshActor.h"
#include "MotionControllerComponent.h"
#include "PIDController3D.h"
#include "MCEnums.h"
#include "MCFinger.h"
#include "MCCharacter.generated.h"


UCLASS()
class UPHYSICSBASEDMC_API AMCCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
    // Sets default values for this character's properties
	AMCCharacter();
	
protected:
	// Called when the games starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	// Select control types, enable/disable tick
	bool CheckHandsAndInitControllers();

	// Left hand skeletal actor
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hands")
	ASkeletalMeshActor* LeftHand;
	// Right hand skeletal actor
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hands")
	ASkeletalMeshActor* RightHand;
	// Visualize MC arrows
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hands")
	bool bTargetArrowsVisible;
	
	// VR Camera and root component
	USceneComponent* VRCameraRoot;	
	UCameraComponent* VRCamera;

	// Left/Right Motion Controllers and root component
	USceneComponent* MCRoot;	
	UMotionControllerComponent* MCLeft;
	UMotionControllerComponent* MCRight;

	// Left/Right MC target arrow visuals
	UArrowComponent* MCLeftTargetArrow;
	UArrowComponent* MCRightTargetArrow;

	/* Control */
	// Left/Right hand location PID controller
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D LeftLocationPIDController;
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D RightLocationPIDController;

	// Left/Right hand rotation control gain
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	float RotationGain;

	// Use the hands current rotation as initial rotation
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	bool bUseHandsCurrentRotationAsInitial;	
	
	// Location / Rotation interaction type (movement type of the hand)
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	EMCLocationControlType LocationControlType;
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	EMCRotationControlType RotationControlType;

	// Left/Right hand rotation offset for hand alignment
	FQuat LeftHandRotationOffset;
	FQuat RightHandRotationOffset;

	// Left/Right skeletal mesh component
	USkeletalMeshComponent* LeftSkeletalMeshComponent;
	USkeletalMeshComponent* RightSkeletalMeshComponent;

	// Control function pointer variable type
	typedef void(AMCCharacter::*ControlFunctionPtrType)(float);

	// Function pointer for the Location / Rotation interaction type
	ControlFunctionPtrType LocationControlFunctionPtr;
	ControlFunctionPtrType RotationControlFunctionPtr;

	// Location interaction functions types
	void LocationControl_AccBased(float InDeltaTime);
	void LocationControl_VelBased(float InDeltaTime);

	// Rotation interaction function types
	void RotationControl_VelBased(float InDeltaTime);
};
