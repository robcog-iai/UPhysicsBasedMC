// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspController.h"
#if ENGINE_MINOR_VERSION >= 19
#include "XRMotionControllerBase.h" // 4.19
#endif

// Constructor
UMCGraspController::UMCGraspController()
{
}

// Init grasp controller
void UMCGraspController::Init(USkeletalMeshComponent* InHand, UMotionControllerComponent* InMC, UInputComponent* InIC)
{
	// Check if InputComponent is given
	if (InIC)
	{
		SetupInputBindings(InMC, InIC);
	}
	else
	{
		// Get the input controller for mapping the grasping control inputs
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			UInputComponent* IC = PC->InputComponent;
			if (IC)
			{
				SetupInputBindings(InMC, IC);
			}
		}
	}
}

// Setup input bindings
void UMCGraspController::SetupInputBindings(UMotionControllerComponent* InMC, UInputComponent* InIC)
{	
	// Check hand type
#if ENGINE_MINOR_VERSION >= 19
	if (InMC->MotionSource == FXRMotionControllerBase::LeftHandSourceId)
#else
	if (InMC->Hand == EControllerHand::Left)
#endif
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Left Bindings setup!"), *FString(__FUNCTION__));
		InIC->BindAxis("LeftGrasp", this, &UMCGraspController::Update);
	}
#if ENGINE_MINOR_VERSION >= 19
	if (InMC->MotionSource == FXRMotionControllerBase::RightHandSourceId)
#else
	if (InMC->Hand == EControllerHand::Right)
#endif
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Right Bindings setup!"), *FString(__FUNCTION__));
		InIC->BindAxis("RightGrasp", this, &UMCGraspController::Update);
	}

}

// Update grasp
void UMCGraspController::Update(const float Val)
{
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

