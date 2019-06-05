// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DGameMode.h"
#include "MC6DHUD.h"

// Ctor
AMC6DGameMode::AMC6DGameMode() : Super()
{
	HUDClass = AMC6DHUD::StaticClass();
}