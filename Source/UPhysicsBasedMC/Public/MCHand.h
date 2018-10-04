// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "MCMovementController6D.h"
#include "MCGraspController.h"
#include "MCFixationGraspController.h"
#include <Net/UnrealNetwork.h>
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "MCHand.generated.h"

/**
 * Hand component with movement and grasping controller
 */
UCLASS()
class UPHYSICSBASEDMC_API UMCHand : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for the component
	UMCHand(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Init hand with the motion controllers
	void Init(UMotionControllerComponent* InMC);

	// List of all bone names
	UPROPERTY(Replicated)
		TArray<FName> ReplicatedBoneNames;

	// List of all bone transforms
	UPROPERTY(Replicated)
		TArray<FTransform> ReplicatedBoneTransforms;

	// A poseable mesh that will mirror the hands movements on the client side
	UPROPERTY(EditAnywhere, Category = "MC")
		UPoseableMeshComponent* PoseableMesh;

	// If a mesh has been attached
	UPROPERTY(Replicated)
		bool HasAttached;

	// Pointer to the attached mesh
	UPROPERTY(Replicated)
		AStaticMeshActor* AttachedMesh;

	// Transform of the attached mesh
	UPROPERTY(Replicated)
		FTransform AttachedTransform;

	// If this is the server
	bool bIsServer = true;

	// Sends information about hands and grasped mesh to the client
	void SendPose();

	// Receives informatin about hands and grasped mesh
	void ReceivePose();

private:
#if WITH_EDITOR
	// Post edit change property callback
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif //WITH_EDITOR

	// Movement controller
	UPROPERTY(EditAnywhere, Category = "MC")
	UMCMovementController6D* MovementController;

	// Grasp controller
	UPROPERTY(EditAnywhere, Category = "MC")
	UMCGraspController* GraspController;
	
	// Enable fixation grasp
	UPROPERTY(EditAnywhere, Category = "MC")
	bool bEnableFixationGrasp;

	// Fixation grasp controller
	UPROPERTY(EditAnywhere, Category = "MC", meta = (editcondition = "bEnableFixationGrasp"))
	UMCFixationGraspController* FixationGraspController;
};
