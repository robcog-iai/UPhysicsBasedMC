// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "MCHand.generated.h"

/**
 * 
 */
UCLASS()
class UPHYSICSBASEDMC_API AMCHand : public ASkeletalMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor
	AMCHand();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;	
};
