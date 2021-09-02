// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MC6DOffset.generated.h"

/**
 * Used as 6D offset for the motion controller tracking
 */
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC 6D Offset") )
class UMC6DCONTROLLER_API UMC6DOffset : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMC6DOffset();

private:
#if WITH_EDITORONLY_DATA
	// Static mesh component of the controller target position offset
	UPROPERTY(EditAnywhere, Category="Semantic Logger")
	class UStaticMeshComponent* ControllerVisual;
#endif // WITH_EDITORONLY_DATA
};
