// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCGraspDataAsset.h"
#include "MCGraspAnimStructs.h"
#include "MCGraspAnimReader.h"
#include "MCGraspExecuter.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "MCGraspController.generated.h"


/**
* Hand type
*/
UENUM()
enum class EMCGraspHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};

UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Grasp Controller") )
class UMCGRASP_API UMCGraspController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCGraspController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:	

	// Initialize this component
	virtual void InitializeComponent();

	// An array the user can fill with grasps they want to use
	UPROPERTY(EditAnywhere)
	TArray<UMCGraspDataAsset*> EquippedGrasps;

	// The input to grasp
	UPROPERTY(EditAnywhere)
	FName GraspAction;

	// The input to select the next grasp
	UPROPERTY(EditAnywhere)
	FName NextGraspAction;

	// The input to select the privious grasp
	UPROPERTY(EditAnywhere)
	FName PreviousGraspAction;

	// tells the executer to apply forces 
	void ApplyForce(const float Input);

	// select next grasp from array
	void NextGrasp();

	// select previous grasp from array
	void PreviousGrasp();

	// Minimum spring value
	UPROPERTY(EditAnywhere)
	float SpringBase = 9000;

	/**
	* Multiplier for the SpringBase. At an input of 1 the base value is multiplied by it.
	* At an input of 0.5 the base value is multiplied by half of it and so on.
	*/
	UPROPERTY(EditAnywhere)
	float SpringMultiplier = 5;

	// Damping value
	UPROPERTY(EditAnywhere)
	float Damping = 1000;

	// Force limit. 0 means no limit. 
	UPROPERTY(EditAnywhere)
	float ForceLimit = 0;

	// Hand type, to listen to the right inputs
	UPROPERTY(EditAnywhere)
	EMCGraspHandType HandType;

private:

	// each controller tries to create an executer, which controlls the parent meshes motors and is saved here. 
	UPROPERTY()
	UMCGraspExecuter* GraspExecuter;

	// binds inputs to functions
	void SetupInputBindings();

	// the array position of the currently selected grasp
	int CurrentGrasp = 0;
};
