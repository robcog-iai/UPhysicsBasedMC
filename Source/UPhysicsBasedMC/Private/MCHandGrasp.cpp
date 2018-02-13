// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCHandGrasp.h"
//#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::Left/RightHandSourceId and GetHandEnumForSourceName()

// Constructor
UMCHandGrasp::UMCHandGrasp()
{
}

// Called when the game starts or when spawned
void UMCHandGrasp::BeginPlay()
{
	Super::BeginPlay();
}

// Init grasp controller
void UMCHandGrasp::Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC)
{
	SetupInputBindings(InMC);
}

// Setup input bindings
void UMCHandGrasp::SetupInputBindings(UMotionControllerComponent* InMC)
{
	//// Get the input controller for mapping the grasping control inputs
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//if (PC)
	//{
	//	UInputComponent* IC = PC->InputComponent;
	//	if (IC)
	//	{
	//		// Check hand type
	//		if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
	//		{
	//			//IC->BindAxis("LeftGrasp", this, &UMCHandGrasp::UpdateGrasp);
	//		}
	//		else if (InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
	//		{
	//			//IC->BindAxis("RightGrasp", this, &UMCHandGrasp::UpdateGrasp);
	//		}
	//	}
	//}
}

//void UVREditorMotionControllerInteractor::PlayHapticEffect(const float Strength)
//{
//	IInputInterface* InputInterface = FSlateApplication::Get().GetInputInterface();
//	if (InputInterface)
//	{
//		const double CurrentTime = FPlatformTime::Seconds();
//
//		//@todo viewportinteration
//		FForceFeedbackValues ForceFeedbackValues;
//		ForceFeedbackValues.LeftLarge = ControllerMotionSource == FXRMotionControllerBase::LeftHandSourceId ? Strength : 0;
//		ForceFeedbackValues.RightLarge = ControllerMotionSource == FXRMotionControllerBase::RightHandSourceId ? Strength : 0;
//
//		// @todo vreditor: If an Xbox controller is plugged in, this causes both the motion controllers and the Xbox controller to vibrate!
//		InputInterface->SetForceFeedbackChannelValues(WorldInteraction->GetMotionControllerID(), ForceFeedbackValues);
//	}
//}

