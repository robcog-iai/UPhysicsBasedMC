// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "PIDController3D.h"


/**
 * 6D static mesh controller callbacks
 */
struct FMC6DControllerCallbacks
{
public:
	// Default constructor
	FMC6DControllerCallbacks() 
	{
		UpdateFunctionPointer = &FMC6DControllerCallbacks::Update_NONE;
	}

	// Destructor
	~FMC6DControllerCallbacks() {}

	// Init
	void Init() {}

	// Update
	void Update(float DeltaTime) 
	{
		(this->*UpdateFunctionPointer)(DeltaTime);
	}

private:
	// Update functions
	void Update_NONE(float DeltaTime) { UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__); }

private:
	// Function pointer type for calling the correct update function
	typedef void(FMC6DControllerCallbacks::*UpdateFunctionPointerType)(float);

	// Function pointer for update
	UpdateFunctionPointerType UpdateFunctionPointer;


};