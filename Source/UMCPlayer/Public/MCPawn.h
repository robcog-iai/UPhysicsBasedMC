// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Motion controller root component (camera and motion controllers will be relative to this)
	USceneComponent* MCRoot;

	// VR Camera
	UCameraComponent* VRCamera;	
};
