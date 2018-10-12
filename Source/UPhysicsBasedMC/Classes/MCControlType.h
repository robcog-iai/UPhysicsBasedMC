// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

/**
* Control type
*/
UENUM()
enum class EMCControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Position				UMETA(DisplayName = "Position"),
	Velocity		 		UMETA(DisplayName = "Velocity"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Force					UMETA(DisplayName = "Force/Torque"),
	Impulse					UMETA(DisplayName = "Impulse"),
};