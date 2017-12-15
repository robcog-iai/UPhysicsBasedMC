// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCHand.h"

// Sets default values
AMCHand::AMCHand()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMCHand::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame, used for motion control
void AMCHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


