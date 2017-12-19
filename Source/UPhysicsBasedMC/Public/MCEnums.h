// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "ObjectMacros.h"


/** Location control type of the hands */
UENUM(BlueprintType)
enum class EMCLocationControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Force					UMETA(DisplayName = "ForceBased"),
	Acceleration			UMETA(DisplayName = "AccelerationBased"),
	Velocity	     		UMETA(DisplayName = "VelocityBased"),
	Position    			UMETA(DisplayName = "PositionBased"),
};

/** Rotation control type of the hands */
UENUM(BlueprintType)
enum class EMCRotationControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Torque					UMETA(DisplayName = "TorqueBased"),
	Acceleration			UMETA(DisplayName = "AccelerationBased"),
	Velocity	     		UMETA(DisplayName = "VelocityBased"),
	Position    			UMETA(DisplayName = "PositionBased"),
};


/** Hand type */
UENUM(BlueprintType)
enum class EMCHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right")
};

/** Finger type */
UENUM(BlueprintType)
enum class EMCFingerType : uint8
{
	Thumb					UMETA(DisplayName = "Thumb"),
	Index					UMETA(DisplayName = "Index"),
	Middle					UMETA(DisplayName = "Middle"),
	Ring					UMETA(DisplayName = "Ring"),
	Pinky					UMETA(DisplayName = "Pinky")
};

/**
* Finger parts
* https://en.wikipedia.org/wiki/Phalanx_bone
*/
UENUM(BlueprintType)
enum class EMCFingerPart : uint8
{
	Metacarpal				UMETA(DisplayName = "Metacarpal"),
	Proximal				UMETA(DisplayName = "Proximal"),
	Intermediate			UMETA(DisplayName = "Intermediate"),
	Distal					UMETA(DisplayName = "Distal")
};
