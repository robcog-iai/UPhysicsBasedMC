// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "ObjectMacros.h"

/**
* Hand type
*/
UENUM()
enum class EMCHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};


/**
* Skeletal type
*/
UENUM()
enum class EMCSkeletalType : uint8
{
	Default					UMETA(DisplayName = "Default"),
	IAI						UMETA(DisplayName = "IAI"),
	Genesis					UMETA(DisplayName = "Genesis"),
};