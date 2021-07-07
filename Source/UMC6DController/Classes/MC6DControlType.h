// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

/**
* Control type
*	Position - teleport with physics on to the given transformation
*	Velocity - set velocity directly: distance(rad) / time, the effect is mass independent
*	Acceleration - set acceleration directly: distance(rad) / time^2, the effect is independent
*	Force - set force(torque): mass * distance(rad) / time^2
*	Impulse - set impulse: mass * distance(rad) / time
*/
UENUM()
enum class EMC6DControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Position				UMETA(DisplayName = "Position"),
	Velocity		 		UMETA(DisplayName = "Velocity"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Force					UMETA(DisplayName = "Force/Torque"),
	Impulse					UMETA(DisplayName = "Impulse"),
};