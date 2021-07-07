// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

/**
* Parallel gripper control type
*	Position - teleport fingers to the target position (useful for debugging logic)
*	LinearDrive - Use the built in constraint driver
*	Acceleration - set acceleration directly: distance(rad) / time^2, the effect is independent
*	Force - set force(torque): mass * distance(rad) / time^2
*/
UENUM()
enum class EMCGripperControlType : uint8
{
	NONE					UMETA(DisplayName = "NONE"),
	Position				UMETA(DisplayName = "Position"),
	LinearDrive				UMETA(DisplayName = "LinearDrive"),
	Acceleration			UMETA(DisplayName = "Acceleration"),
	Force					UMETA(DisplayName = "Force"),
};