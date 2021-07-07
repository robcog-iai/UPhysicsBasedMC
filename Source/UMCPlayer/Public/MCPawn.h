// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "MCPawn.generated.h"

UCLASS(ClassGroup = (MC), meta = (DisplayName = "MC Pawn"))
class UMCPLAYER_API AMCPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMCPawn();

private:
	// Motion controller root component (camera and motion controllers will be relative to this)
	UPROPERTY(EditAnywhere, Category="MC")
	USceneComponent* MCRoot;

	// VR Camera
	UPROPERTY(EditAnywhere, Category = "MC")
	UCameraComponent* VRCamera;
};
