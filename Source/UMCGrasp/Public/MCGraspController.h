// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

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
enum class EMCRealisticGraspHandType : uint8
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

	virtual void InitializeComponent();

	UPROPERTY(EditAnywhere)
	TArray<UMCGraspDataAsset*> EquippedGrasps;

	UPROPERTY(EditAnywhere)
	FName GraspAction;

	UPROPERTY(EditAnywhere)
	FName NextGraspAction;

	UPROPERTY(EditAnywhere)
	FName PreviousGraspAction;

	void ApplyForce(const float Input);

	void NextGrasp();

	void PreviousGrasp();

	UPROPERTY(EditAnywhere)
	float SpringBase = 9000;

	UPROPERTY(EditAnywhere)
	float SpringMultiplier = 5;

	UPROPERTY(EditAnywhere)
	float Damping = 1000;

	UPROPERTY(EditAnywhere)
	float ForceLimit = 0;

	// Hand type, to listen to the right inputs
	UPROPERTY(EditAnywhere)
	EMCRealisticGraspHandType HandType;

private:

	UPROPERTY()
	UMCGraspExecuter* GraspExecuter;

	void SetupInputBindings();

	int CurrentGrasp = 0;
};
