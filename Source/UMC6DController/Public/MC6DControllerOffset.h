// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MC6DControllerOffset.generated.h"

/**
 * Used as 6D offset for the motion controller tracking
 */
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC 6D Controller Offset") )
class UMC6DCONTROLLER_API UMC6DControllerOffset : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMC6DControllerOffset();

private:
#if WITH_EDITORONLY_DATA
	// Static mesh component of the controller target position offset
	class UStaticMeshComponent* ControllerVisual;
#endif // WITH_EDITORONLY_DATA
};
