// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "ObjectMacros.h"

/* Location control type of the hands */
UENUM(BlueprintType)
enum class EMCLocationControlType : uint8
{
	Force					UMETA(DisplayName = "ForceBased"),
	Acceleration			UMETA(DisplayName = "AccelerationBased"),
	Velocity	     		UMETA(DisplayName = "VelocityBased"),
	Position    			UMETA(DisplayName = "PositionBased"),
};

/* Rotation control type of the hands */
UENUM(BlueprintType)
enum class EMCRotationControlType : uint8
{
	Torque					UMETA(DisplayName = "TorqueBased"),
	Acceleration			UMETA(DisplayName = "AccelerationBased"),
	Velocity	     		UMETA(DisplayName = "VelocityBased"),
	Position    			UMETA(DisplayName = "PositionBased"),
};