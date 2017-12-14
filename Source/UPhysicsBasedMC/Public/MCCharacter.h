// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/SkeletalMeshActor.h"
#include "MotionControllerComponent.h"
#include "PIDController3D.h"
#include "MCCharacter.generated.h"

/* Location interaction type of the hands */
UENUM()
enum class EMCLocationInteractionType : uint8
{
	Force					UMETA(DisplayName = "ForceBased"),
	Acceleration			UMETA(DisplayName = "AccelerationBased"),
	Velocity	     		UMETA(DisplayName = "VelocityBased"),
	Position    			UMETA(DisplayName = "PositionBased"),
};

/* Location interaction type of the hands */
UENUM()
enum class EMCRotationInteractionType : uint8
{
	Torque					UMETA(DisplayName = "TorqueBased"),
	Acceleration			UMETA(DisplayName = "AccelerationBased"),
	Velocity	     		UMETA(DisplayName = "VelocityBased"),
	Position    			UMETA(DisplayName = "PositionBased"),
};

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
	// Left hand skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hands")
	ASkeletalMeshActor* LeftHand;
	// Right hand skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hands")
	ASkeletalMeshActor* RightHand;
	// Visualize MC arrows
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Hands")
	bool bTargetArrowsVisible;
	
	// VR Camera root component
	USceneComponent* VRCameraRoot;
	// VR Camera
	UCameraComponent* VRCamera;

	// Motion controller origin parent
	USceneComponent* MCRoot;

	// Left/Right hand motion controller
	UMotionControllerComponent* MCLeft;
	UMotionControllerComponent* MCRight;

	// Left/Right target arrow visual
	UArrowComponent* MCLeftTargetArrow;
	UArrowComponent* MCRightTargetArrow;

	/* Control */
	// Left / Right hand location PID controller
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D LeftLocationPIDController;
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	FPIDController3D RightLocationPIDController;

	// Left / Right hand rotation control gain
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	float RotationGain;

	// Location / Rotation interaction type (movement type of the hand)
	EMCLocationInteractionType MCLocationInteractionType;
	EMCRotationInteractionType MCRotationInteractionType;
	
	// Use the hands current rotation as initial rotation
	UPROPERTY(EditAnywhere, Category = "Physics Based Motion Controller|Control")
	bool bUseHandsCurrentRotationAsInitial;
	
	// Left / Right hand rotation offset
	FQuat LeftHandRotationOffset;
	FQuat RightHandRotationOffset;
};