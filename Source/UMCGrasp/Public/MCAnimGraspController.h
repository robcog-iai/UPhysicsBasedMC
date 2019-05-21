// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCAnimGraspDataAsset.h"
#include "MCAnimGraspExec.h"
#include "MCAnimGraspController.generated.h"

/**
* Hand type
*/
UENUM()
enum class EMCAnimGraspHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};

/**
* The controller chooses the right animations and sends them to the execturor
*/
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Anim Grasp Controller") )
class UMCGRASP_API UMCAnimGraspController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCAnimGraspController();

	// Init the component
	void Init();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

private:
	// Bind user inputs
	void SetupInputBindings();

	// Forward the axis input value to the grasp animation executor
	void Update(float Value);

	// Switch to the next grasp animation
	void NextAnim();

	// Switch to the previous animation
	void PrevAnim();

private:
#if WITH_EDITOR
	// Hand type, to listen to the right inputs
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	EMCAnimGraspHandType HandType;
#endif // WITH_EDITOR

	// The input to grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputAxisName;

	// The input to select the next grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputNextAnimAction;

	// The input to select the previous grasp
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputPrevAnimAction;

	// An array the user can fill with grasps they want to use
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TArray<UMCAnimGraspDataAsset*> AnimGraspDataAssets;

	// Minimum spring value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringBase;

	// Increase the strength of the spring value
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	float SpringMultiplier;

	// Damping value
	UPROPERTY(EditAnywhere)
	float Damping;

	// Force limit, 0 means no limit. 
	UPROPERTY(EditAnywhere)
	float ForceLimit;

	// The executor crunches the number and applies the forces to the fingers
	UPROPERTY()
	UMCAnimGraspExec* AnimGraspExecutor;

	// Currently active grasp animation index
	int CurrAnimGraspIndex;
};
