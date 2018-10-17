// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCParallelGripperController.h"
#include "Kismet/GameplayStatics.h"

void UMCParallelGripperController::Init(EMCGripperControlType ControlType,
	UPhysicsConstraintComponent* LeftFingerConstraint,
	UPhysicsConstraintComponent* RightFingerConstraint)
{
	// Set the user input bindings
	if(UWorld* World = LeftFingerConstraint->GetWorld())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			if (UInputComponent* IC = PC->InputComponent)
			{
				UMCParallelGripperController::SetupInputBindings(IC);
			}
		}
	}

	//LeftFingerConstraint->ConstraintInstance->GetConstraintLocation();

	if (ControlType == EMCGripperControlType::LinearDrive)
	{
		float LeftLim = LeftFingerConstraint->ConstraintInstance.GetLinearLimit();
		LeftFingerConstraint->SetLinearDriveParams(100.f, 50.f, 200.f);
		LeftFingerConstraint->SetLinearPositionDrive(true, false, false);
		LeftFingerConstraint->SetLinearPositionTarget(FVector(LeftLim, 0.f, 0.f));

		float RightLim = RightFingerConstraint->ConstraintInstance.GetLinearLimit();
		RightFingerConstraint->SetLinearDriveParams(100.f, 50.f, 200.f);
		RightFingerConstraint->SetLinearPositionDrive(true, false, false);
		RightFingerConstraint->SetLinearPositionTarget(FVector(RightLim, 0.f, 0.f));
	}

	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
}


void UMCParallelGripperController::SetupInputBindings(UInputComponent* InIC)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d"), TEXT(__FUNCTION__), __LINE__);
	InIC->BindAxis("LeftGrasp", this, &UMCParallelGripperController::UpdateLeft);
	InIC->BindAxis("RightGrasp", this, &UMCParallelGripperController::UpdateRight);
}

// Update function bound to the left input
void UMCParallelGripperController::UpdateLeft(const float Value)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d Value=%f"), TEXT(__FUNCTION__), __LINE__, Value);
}

// Update function bound to the right input
void UMCParallelGripperController::UpdateRight(const float Value)
{
	UE_LOG(LogTemp, Warning, TEXT(">> %s::%d Value=%f"), TEXT(__FUNCTION__), __LINE__, Value);
}