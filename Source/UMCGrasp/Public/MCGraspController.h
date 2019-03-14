// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCGraspDataAsset.h"
#include "MCAnimationDataStructure.h"
#include "MCRead.h"
#include "MCGraspExecuter.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "MCGraspController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMCGRASP_API UMCGraspController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCGraspController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	TArray<UMCGraspDataAsset*> EquippedGrasps;
};