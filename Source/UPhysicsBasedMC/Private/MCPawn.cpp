// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCPawn.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
//#include "XRMotionControllerBase.h"

// Sets default values
AMCPawn::AMCPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Possess player automatically
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Display MC meshes by default
	bVisualizeMCMeshes = true;

	// Crate MC root
	MCRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MCRoot"));
	//MCRoot->SetupAttachment(GetRootComponent());
	RootComponent = MCRoot;
	
	// Create camera component
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(MCRoot);
		
	// Create the right motion controller
	MCLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MCLeft"));
	//MCLeft->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	MCLeft->SetupAttachment(MCRoot);
	
	// Create the left motion controller
	MCRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MCRight"));
	//MCRight->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	MCRight->SetupAttachment(MCRoot);

	//// Create Left MC Hand Component
	//LeftHand = CreateDefaultSubobject<UMCHand>(TEXT("LeftHand"));
	//LeftHand->SetupAttachment(GetRootComponent());

	//// Create Right MC Hand Component
	//RightHand = CreateDefaultSubobject<UMCHand>(TEXT("RightHand"));
	//RightHand->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AMCPawn::BeginPlay()
{
	Super::BeginPlay();

	//// MC meshes visualization
	//MCLeft->bDisplayDeviceModel = bVisualizeMCMeshes;
	//MCRight->bDisplayDeviceModel = bVisualizeMCMeshes;

	//// Init MC Hands
	//LeftHand->Init(MCLeft);
	//RightHand->Init(MCRight);

	//// Disable tick
	//SetActorTickEnabled(false);

	////// Check if VR is enabled
	////IHeadMountedDisplay* HMD = (IHeadMountedDisplay*)(GEngine->XRSystem->GetHMDDevice());
	////if (HMD && HMD->IsHMDEnabled())
	////{
	////	//GEngine->XRSystem->ResetOrientationAndPosition();
	////	GEngine->XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	////	//if (GEngine->XRSystem->GetSystemName().IsEqual("SteamVR"))
	////	//{
	////	//	//MCLeft->SetDisplayModelSource(GEngine->XRSystem->GetSystemName());
	////	//	//MCRight->SetDisplayModelSource(GEngine->XRSystem->GetSystemName());
	////	//	UE_LOG(LogTemp, Warning, TEXT("STEAM VR"));
	////	//}		
	////}
}

// Called every frame
void AMCPawn::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT(" Pawn Tick, Disable"));
}

// Called to bind functionality to input
void AMCPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
