// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "MCFinger.generated.h"

/**
* Enum indicating the finger parts
* https://en.wikipedia.org/wiki/Phalanx_bone
*/
//UENUM(/*BlueprintType*/)
//enum class EFingerBone : uint8
//{
//	Distal			UMETA(DisplayName = "Distal"),
//	Intermediate	UMETA(DisplayName = "Intermediate"),
//	Proximal		UMETA(DisplayName = "Proximal"),
//	Metacarpal		UMETA(DisplayName = "Metacarpal")
//};

/**
* Finger bone type
* https://en.wikipedia.org/wiki/Phalanx_bone
*/
USTRUCT(/*BlueprintType*/)
struct UPHYSICSBASEDMC_API FMCFingerBone
{
	GENERATED_USTRUCT_BODY()

	//// Bone type
	//UPROPERTY(EditAnywhere, Category = "Grasp Control")
	//EFingerBone Type;

	// Bone name
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	FString Name;

	// Bone constraint
	FConstraintInstance* ConstraintInstance;

	// Init finger
	void Init(/*EFingerBone InType, */const FString& InName, FConstraintInstance* InConstraintInstance)
	{
		/*Type = InType;*/
		Name = InName;
		ConstraintInstance = InConstraintInstance;
	}
};

/**
* Hand finger type
*/
USTRUCT()
struct UPHYSICSBASEDMC_API FMCFinger
{
	GENERATED_USTRUCT_BODY()

public:
	// Constructor, set default values
	FMCFinger();

	// Distal bone
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	FMCFingerBone Distal;

	// Intermediate bone
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	FMCFingerBone Intermediate;

	// Proximal bone
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	FMCFingerBone Proximal;

	// Metacarpal bone
	UPROPERTY(EditAnywhere, Category = "Grasp Control")
	FMCFingerBone Metacarpal;

};
