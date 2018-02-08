// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionControllerComponent.h"
#include "PIDController3D.h"
#include "MCHand.generated.h"

/**
* Location control type of the hands 
*/
UENUM()
enum class EMCLocationControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Force					UMETA(DisplayName = "Force"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Impulse					UMETA(DisplayName = "Impulse"),
	Velocity	     		UMETA(DisplayName = "Velocity"),
	Position    			UMETA(DisplayName = "Position"),
};

/**
* Rotation control type of the hands
*/
UENUM()
enum class EMCRotationControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Torque					UMETA(DisplayName = "Torque"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Impulse					UMETA(DisplayName = "Impulse"),
	Velocity	     		UMETA(DisplayName = "Velocity"),
	Position    			UMETA(DisplayName = "Position"),
};

/**
 * 
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCHand : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for the component
	UMCHand();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Init hand with the motion controllers
	void Init(UMotionControllerComponent* MCComp);

private:
	// Motion controller to follow
	UMotionControllerComponent* MotionControllerComp;

	/* Control */
	// Location PID controller
	UPROPERTY(EditAnywhere, Category = "MC|Movement Control")
	FPIDController3D LocationPIDController;

	// Rotation PID controller
	UPROPERTY(EditAnywhere, Category = "MC|Movement Control")
	FPIDController3D RotationPIDController;

	// Location controller type
	UPROPERTY(EditAnywhere, Category = "MC|Movement Control")
	EMCLocationControlType LocationControlType;

	// Rotation controller type
	UPROPERTY(EditAnywhere, Category = "MC|Movement Control")
	EMCRotationControlType RotationControlType;

	// Hand rotation offset for hand alignment
	FQuat HandRotationAlignmentOffset;

	// Control function pointer variable type
	typedef void(UMCHand::*MovementControlFuncPtrType)(float);

	// Function pointer for location movement control
	MovementControlFuncPtrType LocationControlFuncPtr;

	// Function pointer for rotation movement control
	MovementControlFuncPtrType RotationControlFuncPtr;

	// Location interaction functions types
	void LocationControl_None(float InDeltaTime);
	void LocationControl_ForceBased(float InDeltaTime);
	void LocationControl_AccelBased(float InDeltaTime);
	void LocationControl_ImpulseBased(float InDeltaTime);
	void LocationControl_VelBased(float InDeltaTime);
	void LocationControl_PosBased(float InDeltaTime);

	// Rotation interaction function types
	void RotationControl_None(float InDeltaTime);
	void RotationControl_TorqueBased(float InDeltaTime);
	void RotationControl_AccelBased(float InDeltaTime);
	void RotationControl_ImpulseBased(float InDeltaTime);
	void RotationControl_VelBased(float InDeltaTime);
	void RotationControl_PosBased(float InDeltaTime);
};
