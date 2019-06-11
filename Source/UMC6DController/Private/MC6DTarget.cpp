// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MC6DTarget.h"
#include "MC6DOffset.h"
#if WITH_EDITOR
#include "XRMotionControllerBase.h"
#endif // WITH_EDITOR

// Sets default values for this component's properties
UMC6DTarget::UMC6DTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Disable tick, it will be enabled after init
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// Set ticking group 
	//PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;

#if WITH_EDITOR
	bDisplayDeviceModel = true;
	DisplayModelSource = FName("SteamVR");
	bHiddenInGame = true;
	MotionSource = FXRMotionControllerBase::LeftHandSourceId;

	HandType = EMC6DHandType::Left;
	bDynamicController = false;
	EditDelta = 10.f;
	ActiveMovementType = EMC6DMovementTypeSelection::Loc;
	ActiveTerm = EMC6DTermSelection::Delta;
#endif // WITH_EDITOR

	// Default values
	bUseSkeletalMesh = true;
	bApplyToAllSkeletalBodies = false;
	bUseStaticMesh = false;

	// PID values (acc)
	ControlType = EMC6DControlType::Acceleration;
	PLoc = 2000.0f;
	ILoc = 100.0f;
	DLoc = 50.0f;
	MaxLoc = 10000.f;

	PRot = 2000.0f;
	IRot = 100.0f;
	DRot = 400.0f;
	MaxRot = 25000.0f;
}

// Called when the game starts
void UMC6DTarget::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default
	// TODO / ISSUE why does this have to be manually disabled
	// even if PrimaryComponentTick.bStartWithTickEnabled = false;
	// this does not happen with USLVisLegacyManager.cpp
	SetComponentTickEnabled(false);

#if WITH_EDITOR
	if (bDynamicController)
	{
		SetupInputBindings();
	}
#endif // WITH_EDITOR

	// Check if owner has a valid static/skeletal mesh
	if (bUseSkeletalMesh && SkeletalMeshActor)
	{		
		if (USkeletalMeshComponent* SkelMeshComp = SkeletalMeshActor->GetSkeletalMeshComponent())
		{
			// Set mobility and physics parameters
			SkelMeshComp->SetMobility(EComponentMobility::Movable);
			SkelMeshComp->SetSimulatePhysics(true);
			SkelMeshComp->SetEnableGravity(false);

			// Initialize update callbacks with/without offset
			if (UMC6DOffset* OffsetComp = Cast<UMC6DOffset>(SkeletalMeshActor->GetComponentByClass(UMC6DOffset::StaticClass())))
			{
				Controller.Init(this, SkelMeshComp, bApplyToAllSkeletalBodies, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot,
					OffsetComp->GetComponentTransform());
			}
			else
			{
				Controller.Init(this, SkelMeshComp, bApplyToAllSkeletalBodies, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot);
			}
			// Enable Tick
			SetComponentTickEnabled(true);

			// Bind lambda function on the next tick to teleport the mesh to the target location
			// (cannot be done on begin play since the tracking is not active yet)
			// Timer delegate to be able to bind against non UObject functions
			FTimerDelegate TimerDelegateNextTick;
			TimerDelegateNextTick.BindLambda([&, SkelMeshComp]
			{
				// Reset velocity to 0 and teleport to the motion controller location
				// (the controller applies the PID output on the mesh before the lambda is called)
				SkelMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
				SkelMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
				SkelMeshComp->SetWorldTransform(GetComponentTransform(),
					false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			});
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegateNextTick);
		}
	}
	else if (bUseStaticMesh && StaticMeshActor)
	{
		if (UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent())
		{
			// Set mobility and physics parameters
			StaticMeshComp->SetMobility(EComponentMobility::Movable);
			StaticMeshComp->SetSimulatePhysics(true);
			StaticMeshComp->SetEnableGravity(false);

			// Initialize update callbacks with/without offset
			if (UMC6DOffset* OffsetComp = Cast<UMC6DOffset>(StaticMeshActor->GetComponentByClass(UMC6DOffset::StaticClass())))
			{
				Controller.Init(this, StaticMeshComp, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot,
					OffsetComp->GetComponentTransform());
			}
			else
			{
				Controller.Init(this, StaticMeshComp, ControlType,
					PLoc, ILoc, DLoc, MaxLoc, PRot, IRot, DRot, MaxRot);
			}
			// Enable Tick
			SetComponentTickEnabled(true);

			// Bind lambda function on the next tick to teleport the mesh to the target location
			// (cannot be done on begin play since the tracking is not active yet)
			// Timer delegate to be able to bind against non UObject functions
			FTimerDelegate TimerDelegateNextTick;
			TimerDelegateNextTick.BindLambda([&, StaticMeshComp]
			{
				// Reset velocity to 0 and teleport to the motion controller location
				// (the controller applies the PID output on the mesh before the lambda is called)
				StaticMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
				StaticMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
				StaticMeshComp->SetWorldTransform(GetComponentTransform(),
				 false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			});
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegateNextTick);
		}
	}
	// Could not set the update controller, tick remains disabled
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMC6DTarget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Radio button style between bUseSkeletalMesh, bUseStaticMesh
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, bUseSkeletalMesh))
	{
		if (bUseSkeletalMesh)
		{
			bUseStaticMesh = false;
			if (StaticMeshActor)
			{
				StaticMeshActor = nullptr;
			}
		}
		else 
		{
			bUseStaticMesh = true;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, bUseStaticMesh))
	{
		if (bUseStaticMesh)
		{
			bUseSkeletalMesh = false;
			if (SkeletalMeshActor)
			{
				SkeletalMeshActor = nullptr;
			}
		}
		else
		{
			bUseSkeletalMesh = true;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, HandType))
	{
		if (HandType == EMC6DHandType::Left)
		{
			MotionSource = FXRMotionControllerBase::LeftHandSourceId;
		}
		else if (HandType == EMC6DHandType::Right)
		{
			MotionSource = FXRMotionControllerBase::RightHandSourceId;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, ControlType))
	{
		if (ControlType == EMC6DControlType::Velocity)
		{
			PLoc = 10.0f;
			ILoc = 0.1f;
			DLoc = 1.0f;
			MaxLoc = 20.f;

			PRot = 12.0f;
			IRot = 0.1f;
			DRot = 1.0f;
			MaxRot = 15.f;
		}
		else if (ControlType == EMC6DControlType::Acceleration || ControlType == EMC6DControlType::Force)
		{
			PLoc = 2000.0f;
			ILoc = 100.0f;
			DLoc = 50.0f;
			MaxLoc = 10000.f;

			PRot = 2000.0f;
			IRot = 100.0f;
			DRot = 400.0f;
			MaxRot = 25000.0f;
		}
	}
}
#endif // WITH_EDITOR

// Called every frame
void UMC6DTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Apply target location to the referenced mesh
	Controller.Update(DeltaTime);

#if UMC_WITH_CHART
	Controller.GetDebugChartData(ChartData.LocErr, ChartData.LocPID, ChartData.RotErr, ChartData.RotPID);
#endif UMC_WITH_CHART
}

// Reset the location PID
void  UMC6DTarget::ResetLocationPID(bool bClearErrors /* = true*/)
{
	Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc, bClearErrors);
}

// Reset the location PID
void  UMC6DTarget::ResetRotationPID(bool bClearErrors /* = true*/)
{
	Controller.ResetRot(PRot, IRot, DRot, MaxRot, bClearErrors);
}

#if WITH_EDITOR
// Input for changing the PID values on the fly
void UMC6DTarget::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAction("SwitchMovementType", IE_Pressed, this, &UMC6DTarget::SwitchMovementType);
			IC->BindAction("SwitchTerm", IE_Pressed, this, &UMC6DTarget::SwitchTerm);
			IC->BindAction("SelectP", IE_Pressed, this, &UMC6DTarget::SelectP);
			IC->BindAction("SelectI", IE_Pressed, this, &UMC6DTarget::SelectI);
			IC->BindAction("SelectD", IE_Pressed, this, &UMC6DTarget::SelectD);
			IC->BindAction("SelectMax", IE_Pressed, this, &UMC6DTarget::SelectMax);
			IC->BindAction("SelectDelta", IE_Pressed, this, &UMC6DTarget::SelectDelta);
			IC->BindAction("IncSelection", IE_Pressed, this, &UMC6DTarget::IncSelection);
			IC->BindAction("DecSelection", IE_Pressed, this, &UMC6DTarget::DecSelection);
		}
	}
}

void UMC6DTarget::SwitchMovementType()
{
	if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
	{
		ActiveMovementType = EMC6DMovementTypeSelection::Rot;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ROT")));
	}
	else
	{
		ActiveMovementType = EMC6DMovementTypeSelection::Loc;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("LOC")));
	}
}

void UMC6DTarget::SwitchTerm()
{
	if (ActiveTerm == EMC6DTermSelection::P)
	{
		ActiveTerm = EMC6DTermSelection::I;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("I")));
	}
	else if (ActiveTerm == EMC6DTermSelection::I)
	{
		ActiveTerm = EMC6DTermSelection::D;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("D")));
	}
	else if (ActiveTerm == EMC6DTermSelection::D)
	{
		ActiveTerm = EMC6DTermSelection::Max;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Max")));
	}
	else if (ActiveTerm == EMC6DTermSelection::Max)
	{
		ActiveTerm = EMC6DTermSelection::Delta;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Delta")));
	}
	else if (ActiveTerm == EMC6DTermSelection::Delta)
	{
		ActiveTerm = EMC6DTermSelection::P;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("P")));
	}
}

void UMC6DTarget::SelectP()
{
	ActiveTerm = EMC6DTermSelection::P;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("P")));
}

void UMC6DTarget::SelectI()
{
	ActiveTerm = EMC6DTermSelection::I;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("I")));
}

void UMC6DTarget::SelectD()
{
	ActiveTerm = EMC6DTermSelection::D;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("D")));
}

void UMC6DTarget::SelectMax()
{
	ActiveTerm = EMC6DTermSelection::Max;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Max")));
}

void UMC6DTarget::SelectDelta()
{
	ActiveTerm = EMC6DTermSelection::Delta;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Delta")));
}

void UMC6DTarget::IncSelection()
{
	switch (ActiveTerm)
	{
	case EMC6DTermSelection::P :
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			PLoc += EditDelta;
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("PLoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					PLoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			PRot += EditDelta;
			Controller.ResetRot(PRot, IRot, DRot, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("PRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					PRot, PRot, IRot, DRot, MaxRot, EditDelta));
		}
		break;
	case EMC6DTermSelection::I :
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			ILoc += EditDelta;
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("ILoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					ILoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			IRot += EditDelta;
			Controller.ResetRot(PRot, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("IRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					IRot, PRot, IRot, DRot, MaxRot, EditDelta));
		}
		break;
	case EMC6DTermSelection::D:
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			DLoc += EditDelta;
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("DLoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					DLoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			DRot += EditDelta;
			Controller.ResetRot(PRot, IRot, DRot, MaxRot);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("DRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					DRot, PRot, ILoc, DLoc, MaxLoc, EditDelta));
		}
		break;
	case EMC6DTermSelection::Max:
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			MaxLoc += EditDelta;
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("MaxLoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					MaxLoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			MaxRot += EditDelta;
			Controller.ResetRot(PRot, IRot, DRot, MaxRot);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("MaxRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					MaxRot, PRot, ILoc, DLoc, MaxLoc, EditDelta));
		}
		break;
	case EMC6DTermSelection::Delta:
			EditDelta *= 10.f;
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("Delta=%.1f"), EditDelta));
		break;
	}

}

void UMC6DTarget::DecSelection()
{
	switch (ActiveTerm)
	{
	case EMC6DTermSelection::P:
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			PLoc -= EditDelta;
			if (PLoc < 0.f)
			{
				PLoc = 0.f;
			}
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("PLoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					PLoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			PRot -= EditDelta;
			if (PRot < 0.f)
			{
				PRot = 0.f;
			}
			Controller.ResetLoc(PRot, IRot, DRot, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("PRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					PRot, PRot, IRot, DRot, MaxRot, EditDelta));
		}
		break;
	case EMC6DTermSelection::I:
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			ILoc -= EditDelta;
			if (ILoc < 0.f)
			{
				ILoc = 0.f;
			}
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("ILoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					ILoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			IRot -= EditDelta;
			if (IRot < 0.f)
			{
				IRot = 0.f;
			}
			Controller.ResetLoc(PRot, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("IRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					IRot, PRot, IRot, DRot, MaxRot, EditDelta));
		}
		break;
	case EMC6DTermSelection::D:
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			DLoc -= EditDelta;
			if (DLoc < 0.f)
			{
				DLoc = 0.f;
			}
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("DLoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					DLoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			DRot -= EditDelta;
			if (DRot < 0.f)
			{
				DRot = 0.f;
			}
			Controller.ResetLoc(PRot, IRot, DRot, MaxRot);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("DRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					DRot, PRot, ILoc, DLoc, MaxLoc, EditDelta));
		}
		break;
	case EMC6DTermSelection::Max:
		if (ActiveMovementType == EMC6DMovementTypeSelection::Loc)
		{
			MaxLoc -= EditDelta;
			if (MaxLoc < 0.f)
			{
				MaxLoc = 0.f;
			}
			Controller.ResetLoc(PLoc, ILoc, DLoc, MaxLoc);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("MaxLoc:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					MaxLoc, PLoc, ILoc, DLoc, MaxLoc, EditDelta));
		}
		else
		{
			MaxRot -= EditDelta;
			if (MaxRot < 0.f)
			{
				MaxRot = 0.f;
			}
			Controller.ResetLoc(PRot, IRot, DRot, MaxRot);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
				FString::Printf(TEXT("MaxRot:%.1f;  [P=%.1f; I=%.1f; D=%.1f; Max=%.1f; Delta=%.1f;]"),
					MaxRot, PRot, ILoc, DLoc, MaxLoc, EditDelta));
		}
		break;
	case EMC6DTermSelection::Delta:
		EditDelta *= 0.1f;
		if (EditDelta < 0.1f)
		{
			EditDelta = 0.1f;
		}
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("Delta=%.1f"), EditDelta));
		break;
	}
}
#endif // WITH_EDITOR