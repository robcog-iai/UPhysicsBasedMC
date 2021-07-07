// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
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

	bIgnore = false;
	bIsInit = false;
	bIsStarted = false;
	bIsFinished = false;

#if WITH_EDITORONLY_DATA
	bDisplayDeviceModel = true;
	DisplayModelSource = FName("SteamVR");
	bHiddenInGame = true;
	MotionSource = FXRMotionControllerBase::LeftHandSourceId;

	HandType = EMC6DHandType::Left;
#endif // WITH_EDITORONLY_DATA

	// Default values
	bUseSkeletalMesh = true;
	bApplyToAllSkeletalBodies = false;

	// PID values (acc)
	LocControlType = EMC6DControlType::Acceleration;
	PLoc = DEF_PLoc_Acc;
	ILoc = DEF_ILoc_Acc;
	DLoc = DEF_DLoc_Acc;
	MaxLoc = DEF_MaxLoc_Acc;

	RotControlType = EMC6DControlType::Velocity;
	PRot = DEF_PRot_Vel;
	IRot = DEF_IRot_Vel;
	DRot = DEF_DRot_Vel;
	MaxRot = DEF_MaxRot_Vel;
}

// Called when the game starts
void UMC6DTarget::BeginPlay()
{
	Super::BeginPlay();

	// Disable tick by default
	SetComponentTickEnabled(false);


	if (bIgnore)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s's ignore flag is true, skipping.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}

	Init();

	//Start();
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &UMC6DTarget::Start, StartDelay, false);
}

// Called when actor removed from game or game ended
void UMC6DTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (!bIsFinished && (bIsStarted || bIsInit))
	{
		Finish();
	}
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMC6DTarget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, HandType))
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
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, LocControlType))
	{
		if (LocControlType == EMC6DControlType::Velocity)
		{
			PLoc = DEF_PLoc_Vel;
			ILoc = DEF_ILoc_Vel;
			DLoc = DEF_DLoc_Vel;
			MaxLoc = DEF_MaxLoc_Vel;
		}
		else if (LocControlType == EMC6DControlType::Acceleration || LocControlType == EMC6DControlType::Force)
		{
			PLoc = DEF_PLoc_Acc;
			ILoc = DEF_ILoc_Acc;
			DLoc = DEF_DLoc_Acc;
			MaxLoc = DEF_MaxLoc_Acc;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, RotControlType))
	{
		if (RotControlType == EMC6DControlType::Velocity)
		{
			PRot = DEF_PRot_Vel;
			IRot = DEF_IRot_Vel;
			DRot = DEF_DRot_Vel;
			MaxRot = DEF_MaxRot_Vel;
		}
		else if (RotControlType == EMC6DControlType::Acceleration || RotControlType == EMC6DControlType::Force)
		{
			PRot = DEF_PRot_Acc;
			IRot = DEF_IRot_Acc;
			DRot = DEF_DRot_Acc;
			MaxRot = DEF_MaxRot_Acc;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMC6DTarget, bUpdateLocationButtonHack))
	{
		bUpdateLocationButtonHack = false;
		if (!OverwriteSkeletalMeshActor || !OverwriteSkeletalMeshActor->IsValidLowLevel() || OverwriteSkeletalMeshActor->IsPendingKillOrUnreachable())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s::%d %s::%s's OverwriteSkeletalMeshActor is not valid.."),
				*FString(__FUNCTION__), __LINE__, *GetOwner()->GetName(), *GetName());
			return;
		}

		if (OverwriteSkeletalMeshActor->GetSkeletalMeshComponent()->GetBoneIndex(OverwriteBoneName) == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s::%d %s::%s's OverwriteSkeletalMeshActor selected bone %s is not valid.."),
				*FString(__FUNCTION__), __LINE__, *GetOwner()->GetName(), *GetName(), *OverwriteBoneName.ToString());
			return;
		}

		if (!SkeletalMeshActor || !SkeletalMeshActor->IsValidLowLevel() || SkeletalMeshActor->IsPendingKillOrUnreachable())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s::%d %s::%s's SkeletalMeshActor is not valid.."),
				*FString(__FUNCTION__), __LINE__, *GetOwner()->GetName(), *GetName());
			return;
		}

		const FVector MoveToLocation = OverwriteSkeletalMeshActor->GetSkeletalMeshComponent()->GetBoneLocation(OverwriteBoneName);
		SkeletalMeshActor->SetActorLocation(MoveToLocation);
	}
}
#endif // WITH_EDITOR

// Called every frame
void UMC6DTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Apply target location to the referenced mesh
	Controller.UpdateController(DeltaTime);

#if UMC_WITH_CHART
	Controller.GetDebugChartData(ChartData.LocErr, ChartData.LocPID, ChartData.RotErr, ChartData.RotPID);
#endif // UMC_WITH_CHART
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

// Check references
void UMC6DTarget::Init()
{
	if (bIsInit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s is already initialized.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}

	// Check if the target location should and can be overwritten
	if (bOverwriteTargetLocation && OverwriteSkeletalMeshActor)
	{
		// Check if the bone exist
		if (OverwriteSkeletalMeshActor->GetSkeletalMeshComponent()->GetBoneIndex(OverwriteBoneName) == INDEX_NONE)
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d %s' %s has no bone named %s, location overwriting will not work.."),
				*FString(__FUNCTION__), __LINE__, *GetName(),
				*OverwriteSkeletalMeshActor->GetName(), *OverwriteBoneName.ToString());
			bOverwriteTargetLocation = false;
		}
	}

	// Check if owner has a valid static/skeletal mesh
	if (bUseSkeletalMesh && SkeletalMeshActor)
	{
		if (USkeletalMeshComponent* SkelMeshComp = SkeletalMeshActor->GetSkeletalMeshComponent())
		{
			// Set mobility and physics parameters
			//SkelMeshComp->SetMobility(EComponentMobility::Movable);
			//SkelMeshComp->SetSimulatePhysics(true);
			//SkelMeshComp->SetEnableGravity(false);

			// Initialize update callbacks with/without offset
			if (UMC6DOffset* OffsetComp = Cast<UMC6DOffset>(SkeletalMeshActor->GetComponentByClass(UMC6DOffset::StaticClass())))
			{
				Controller.Init(this, SkelMeshComp, bApplyToAllSkeletalBodies, LocControlType,
					PLoc, ILoc, DLoc, MaxLoc, RotControlType, PRot, IRot, DRot, MaxRot,
					OffsetComp->GetComponentTransform());
			}
			else
			{
				Controller.Init(this, SkelMeshComp, bApplyToAllSkeletalBodies, LocControlType,
					PLoc, ILoc, DLoc, MaxLoc, RotControlType, PRot, IRot, DRot, MaxRot);
			}

			// Let the controler know that the location should be overwritten
			if (bOverwriteTargetLocation)
			{
				if (OverwriteSkeletalMeshActor)
				{
					Controller.OverwriteToUseBoneForTargetLocation(
						OverwriteSkeletalMeshActor->GetSkeletalMeshComponent(), OverwriteBoneName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("%s::%d  no overwrite skeletal mesh actor selected, aborting.."),
						*FString(__FUNCTION__), __LINE__);
					return;
				}
			}

			bIsInit = true;
			UE_LOG(LogTemp, Warning, TEXT("%s::%d %s succesfully initialized.."),
				*FString(__FUNCTION__), __LINE__, *GetName());
		}
	}
	else if (StaticMeshActor)
	{
		if (UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent())
		{
			// Set mobility and physics parameters
			//StaticMeshComp->SetMobility(EComponentMobility::Movable);
			//StaticMeshComp->SetSimulatePhysics(true);
			//StaticMeshComp->SetEnableGravity(false);

			// Initialize update callbacks with/without offset
			if (UMC6DOffset* OffsetComp = Cast<UMC6DOffset>(StaticMeshActor->GetComponentByClass(UMC6DOffset::StaticClass())))
			{
				Controller.Init(this, StaticMeshComp, LocControlType,
					PLoc, ILoc, DLoc, MaxLoc, RotControlType, PRot, IRot, DRot, MaxRot,
					OffsetComp->GetComponentTransform());
			}
			else
			{
				Controller.Init(this, StaticMeshComp, LocControlType,
					PLoc, ILoc, DLoc, MaxLoc, RotControlType, PRot, IRot, DRot, MaxRot);
			}

			// Let the controler know that the location should be overwritten
			if (bOverwriteTargetLocation)
			{
				Controller.OverwriteToUseBoneForTargetLocation(
					OverwriteSkeletalMeshActor->GetSkeletalMeshComponent(), OverwriteBoneName);
			}
		}

		bIsInit = true;
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s succesfully initialized.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
	}
}

// Start controller
void UMC6DTarget::Start()
{
	if (bIsStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s is already started.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}

	if (!bIsInit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s is not initialized, cannot start.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}

	if (bUseSkeletalMesh && SkeletalMeshActor)
	{		
		if (USkeletalMeshComponent* SkelMeshComp = SkeletalMeshActor->GetSkeletalMeshComponent())
		{
			// Check if any external manager disabled physics on the world
			if (SkelMeshComp->IsSimulatingPhysics() 
				&& (SkelMeshComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision))
			{
				// Teleport hands to MC pose, it might not work since at the beginning the tracking is off
				TeleportToInitialPose();

				SetComponentTickEnabled(true);

				// Teleport again using a delay
				FTimerHandle DummyHandle;
				GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &UMC6DTarget::TeleportToInitialPose, 0.5f, false);

				bIsStarted = true;
				UE_LOG(LogTemp, Warning, TEXT("%s::%d::%.4f %s succesfully started.."),
					*FString(__FUNCTION__), __LINE__, GetWorld()->GetTimeSeconds(), *GetName());
				return;
			}

			//// Bind lambda function on the next tick to teleport the mesh to the target location
			//// (cannot be done on begin play since the tracking is not active yet)
			//// Timer delegate to be able to bind against non UObject functions
			//FTimerDelegate TimerDelegateNextTick;
			//TimerDelegateNextTick.BindLambda([&, SkelMeshComp]
			//	{
			//		// Check if any external manager disabled physics on the world
			//		if (!SkelMeshComp->IsSimulatingPhysics())
			//		{
			//			UE_LOG(LogTemp, Error, TEXT("%s::%d "), *FString(__FUNCTION__), __LINE__);
			//			Controller.Clear();
			//			// If the skeletal mesh component physics was disabled, disable the controller
			//			SetComponentTickEnabled(false);
			//		}
			//		else
			//		{
			//			UE_LOG(LogTemp, Warning, TEXT("%s::%d "), *FString(__FUNCTION__), __LINE__);
			//			// Reset velocity to 0 and teleport to the motion controller location
			//			// (the controller applies the PID output on the mesh before the lambda is called)
			//			SkelMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
			//			SkelMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
			//			SkelMeshComp->SetWorldTransform(GetComponentTransform(),
			//				false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			//		}
			//	});
			//GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegateNextTick);
		}
	}
	else if (StaticMeshActor)
	{
		if (UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent())
		{
			// Check if any external manager disabled physics on the world
			if (StaticMeshComp->IsSimulatingPhysics() 
				&& (StaticMeshComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision))
			{
				// Teleport hands to MC pose, it might not work since at the beginning the tracking is off
				TeleportToInitialPose();

				SetComponentTickEnabled(true);

				// Teleport again using a delay
				FTimerHandle DummyHandle;
				GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &UMC6DTarget::TeleportToInitialPose, 0.5f, false);

				bIsStarted = true;
				UE_LOG(LogTemp, Warning, TEXT("%s::%d::%.4f %s succesfully started.."),
					*FString(__FUNCTION__), __LINE__, GetWorld()->GetTimeSeconds(), *GetName());
				return;
			}

			//// Bind lambda function on the next tick to teleport the mesh to the target location
			//// (cannot be done on begin play since the tracking is not active yet)
			//// Timer delegate to be able to bind against non UObject functions
			//FTimerDelegate TimerDelegateNextTick;
			//TimerDelegateNextTick.BindLambda([&, StaticMeshComp]
			//	{
			//		// Check if any external manager disabled physics on the world
			//		if (!StaticMeshComp->IsSimulatingPhysics())
			//		{
			//			Controller.Clear();
			//			// If the skeletal mesh component physics was disabled, disable the controller
			//			SetComponentTickEnabled(false);
			//		}
			//		else
			//		{
			//			// Reset velocity to 0 and teleport to the motion controller location
			//			// (the controller applies the PID output on the mesh before the lambda is called)
			//			StaticMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
			//			StaticMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
			//			StaticMeshComp->SetWorldTransform(GetComponentTransform(),
			//				false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			//		}
			//	});
			//GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegateNextTick);
		}
	}
	UE_LOG(LogTemp, Error, TEXT("%s::%d::%.4f %s could not bet started.."),
		*FString(__FUNCTION__), __LINE__, GetWorld()->GetTimeSeconds(), *GetName());
}

// Stop the controller
void UMC6DTarget::Finish(bool bForced)
{
	if (bIsFinished)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s is already finished.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}

	if (!bIsInit && !bIsStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s is not initialized nor started, cannot finish.."),
			*FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}

	SetComponentTickEnabled(false);

	bIsStarted = false;
	bIsInit = false;
	bIsFinished = true;
	UE_LOG(LogTemp, Warning, TEXT("%s::%d %s succesfully finished.."),
		*FString(__FUNCTION__), __LINE__, *GetName());
}

// Initial teleport the hands to the motion controller location, 
// has to be called after a delay since at begin play the controller is not tracked yet
void UMC6DTarget::TeleportToInitialPose()
{
	UE_LOG(LogTemp, Warning, TEXT("%s::%d::%.4fs"), *FString(__FUNCTION__), __LINE__, GetWorld()->GetTimeSeconds());
	if (bUseSkeletalMesh && SkeletalMeshActor)
	{
		if (USkeletalMeshComponent* SkelMeshComp = SkeletalMeshActor->GetSkeletalMeshComponent())
		{
			// Check if any external manager disabled physics on the world
			if (SkelMeshComp->IsSimulatingPhysics()
				&& (SkelMeshComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision))
			{
				SkelMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
				SkelMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
				SkelMeshComp->SetWorldTransform(GetComponentTransform(),
					false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
	else if (StaticMeshActor)
	{
		if (UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent())
		{
			// Check if any external manager disabled physics on the world
			if (StaticMeshComp->IsSimulatingPhysics()
				&& (StaticMeshComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision))
			{
				StaticMeshComp->SetPhysicsLinearVelocity(FVector(0.f));
				StaticMeshComp->SetPhysicsAngularVelocityInRadians(FVector(0.f));
				StaticMeshComp->SetWorldTransform(GetComponentTransform(),
					false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}

