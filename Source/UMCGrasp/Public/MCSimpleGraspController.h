// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/ConstraintDrives.h"
#include "MCSimpleGraspController.generated.h"

/**
* Hand type
*/
UENUM()
enum class EMCSimpleGraspHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};

/**
* Skeltal type
*/
UENUM()
enum class EMCSimpleGraspSkeletalType : uint8
{
	Default					UMETA(DisplayName = "Default"),
	Genesis					UMETA(DisplayName = "Genesis"),
};

/**
 * Skeletal grasp controller
 */
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Simple Grasp Controller"))
class UMCGRASP_API UMCSimpleGraspController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCSimpleGraspController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // 

private:
	// Init the controller
	void Init();

	// Update the grasp
	void Update(float Value);

	// Update the grasp for the genesis hands
	void Update_Genesis(float Value);

private:
	// Hand type, to listen to the right inputs
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	EMCSimpleGraspHandType HandType;

	// Skeletal type, to apply the correct angles and bones
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	EMCSimpleGraspSkeletalType SkeletalType;

	// Input axis name
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputAxisName;

	// Angular drive mode
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TEnumAsByte<EAngularDriveMode::Type> AngularDriveMode;

	// Max angle target (the angle of the constraint when the trigger is at max (1.f)).
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float MaxAngleMultiplier;

	// Spring value to apply to the angular drive (Position strength)
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float Spring;

	// Damping value to apply to the angular drive (Velocity strength) 
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float Damping;

	// Limit of the force that the angular drive can apply
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float ForceLimit;

	// Skeletal mesh of the owner
	class USkeletalMeshComponent* SkeletalMesh;

	// Don't iterate over the constraints if the input value did not change since last time
	float PrevInputVal;
};
