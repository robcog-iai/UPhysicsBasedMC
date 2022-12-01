// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "MCGraspAnimController.h"
#include "ManusBlueprintLibrary.h"
#include "Manus.h"
#include "ManusLiveLinkSource.h"
#include "ManusLiveLinkRemapAsset.h"
#include "ManusLiveLinkRemapAsset.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "EngineUtils.h" 
#include "Animation/SkeletalMeshActor.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UMCGraspAnimController::UMCGraspAnimController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIgnore = false;
	bLogDebug = false;

#if WITH_EDITOR
	HandType = EMCGraspAnimHandType::Left;
#endif // WITH_EDITOR

	// Default parameters
	InputAxisName = "LeftGrasp";
	InputNextAnimAction = "LeftNextGraspAnim";
	InputPrevAnimAction = "LeftPrevGraspAnim";

	SpringIdle = 1000000000.f;
	TriggerStrength = 5.f;
	bDecreaseStrength = false;
	Damping = 100000000.f;
	ForceLimit = 0.f;
	ActiveAnimIdx = INDEX_NONE;
	bIsIdle = true;
	bIsMax = false;

	PositionStrengh = 100000.0;
	PositionDamping = 1000.0;
	OrientationStrengh = 1000000.0;
	OrientationDamping = 10000.0;

}

// Called when the game starts
void UMCGraspAnimController::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

#if WITH_EDITOR
// Called when a property is changed in the editor
void UMCGraspAnimController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the changed property name
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ?
		PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Set the left / right constraint actors
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMCGraspAnimController, HandType))
	{
		if (HandType == EMCGraspAnimHandType::Left)
		{
			InputAxisName = "LeftGrasp";
			InputNextAnimAction = "LeftNextGraspAnim";
			InputPrevAnimAction = "LeftPrevGraspAnim";
		}
		else if (HandType == EMCGraspAnimHandType::Right)
		{
			InputAxisName = "RightGrasp";
			InputNextAnimAction = "RightNextGraspAnim";
			InputPrevAnimAction = "RightPrevGraspAnim";
		}
	}
}
#endif // WITH_EDITOR

// Init component
void UMCGraspAnimController::Init()
{
	if (bIgnore)
	{
		return;
	}

	if (!LoadSkeletalMesh())
	{
		return;
	}

	if (!LoadAnimationData() && !bUseManus)
	{
		return;
	}

	// The active spring value is at least the value of idle (this increases when the trigger is pressed)
	SpringActive = SpringIdle;

	if (!bUseManus) {
		// Go to the first animations
		ActiveAnimIdx = 0;
		ActiveAnimation = Animations[0];
		ActiveAnimStepSize = 1.f / static_cast<float>(ActiveAnimation.Num() - 1);

		OnGraspType.Broadcast(AnimationNames[ActiveAnimIdx]);

		DriveToFirstFrame();
	}

	//if (HandType == EMCGraspAnimHandType::Left)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("L:%s"),
	//		*AnimationNames[ActiveAnimIdx]), true, FVector2D(1.5f, 1.5f));
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("R:%s"),
	//		*AnimationNames[ActiveAnimIdx]), true, FVector2D(1.5f, 1.5f));
	//}

	// Set and drive to idle

	if (bUseManus) {
		WorldConstraint = Cast<UPhysicsConstraintComponent>(GetOwner()->GetComponentByClass(UPhysicsConstraintComponent::StaticClass()));
		if (WorldConstraint) {
			if (WorldConstraint->ConstraintActor1 != GetOwner()) {
				// The first constrained actor has to be the hand 
				AActor* tmp = WorldConstraint->ConstraintActor1;
				WorldConstraint->ConstraintActor1 = WorldConstraint->ConstraintActor2;
				WorldConstraint->ConstraintActor2 = tmp;
				WorldConstraint->InitComponentConstraint();
				//WorldConstraint->SetConstrainedComponents()
				UE_LOG(LogTemp, Warning, TEXT("The Constrained Actors where switched."))
			}
			InitWorldConstraint(WorldConstraint);

		}
		else {
			GetOwner()->Modify();

			UPhysicsConstraintComponent* NewConstraint = NewObject<UPhysicsConstraintComponent>(GetOwner(), "WorldPhysicsConstraint", RF_Transactional);
			// Find the static mesh actor for the floor
			AActor* FloorActor = nullptr;
			for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
				if (ActorItr->GetName().ToLower().Contains("cube")) {
					FloorActor = *ActorItr;
					break;
				}
			}
			if (FloorActor != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("Floor Actor: %s"), *FloorActor->GetName());
			}

			NewConstraint->ConstraintActor1 = GetOwner();
			NewConstraint->ConstraintActor2 = FloorActor;

			//NewConstraint->ComponentName1 = GetOwner()->GetRootComponent()->GetName();

			//NewConstraint->SetConstrainedComponents(Cast<UPrimitiveComponent>(GetOwner()), "", Cast<UPrimitiveComponent>(FloorActor), "");

			GetOwner()->AddInstanceComponent(NewConstraint);
			GetOwner()->AddOwnedComponent(NewConstraint);

			NewConstraint->RegisterComponent();

			GetOwner()->RerunConstructionScripts();

			WorldConstraint = NewConstraint;
			InitWorldConstraint(NewConstraint);
			NewConstraint->InitComponentConstraint();

		}
	}

	ManusSkel = ManusHand->GetSkeletalMeshComponent();

	//FManusModule::Get().StartupModule();
	//FManusModule::Get().SetActive(true);
	//TSharedPtr<ILiveLinkSource> Source = FManusModule::Get().GetLiveLinkSource(EManusLiveLinkSourceType::Local);
	//FManusLiveLinkSource* Source = new FManusLiveLinkSource(EManusLiveLinkSourceType::Local);
	//FManusLiveLinkSource::GetManusLiveLinkUserLiveLinkSubjectName(0);

	// Bind the user input to the callbacks
	SetupInputBindings();

}

void UMCGraspAnimController::InitWorldConstraint(UPhysicsConstraintComponent* WorldConstraint) {
	WorldConstraint->SetLinearPositionDrive(true, true, true);
	//WorldConstraint->SetLinearVelocityDrive(true, true, true);

	WorldConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0);
	WorldConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0);
	WorldConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0);

	WorldConstraint->SetLinearDriveParams(PositionStrengh, PositionDamping, 0);

	WorldConstraint->SetAngularSwing1Limit(ACM_Free, 0);
	WorldConstraint->SetAngularSwing2Limit(ACM_Free, 0);
	WorldConstraint->SetAngularTwistLimit(ACM_Free, 0);

	WorldConstraint->SetAngularVelocityDriveTwistAndSwing(false, false);

	WorldConstraint->SetAngularDriveMode(EAngularDriveMode::SLERP);
	WorldConstraint->SetOrientationDriveSLERP(true);
	//WorldConstraint->SetAngularVelocityDriveSLERP(true);

	WorldConstraint->SetAngularDriveParams(OrientationStrengh, OrientationDamping, 0);
}

void UMCGraspAnimController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {

	TArray<FName> BoneNames;
	SkelComp->GetBoneNames(BoneNames);
	TArray<FQuat> BoneLocations;
	FTransform W_T_Hand = ManusHand->GetTransform().Inverse();

	for (int i = 0; i < ManusSkel->Constraints.Num(); i++) {
		if (ManusSkel->Constraints[i]->ConstraintBone1.ToString().Contains("Carpal")) {
			continue;
		}
		//FTransform B1 = ManusSkel->GetBoneTransform(ManusSkel->GetBoneIndex(ManusSkel->Constraints[i]->ConstraintBone1));
		FTransform B2_Component = FTransform(ManusSkel->GetBoneQuaternion(ManusSkel->Constraints[i]->ConstraintBone2, EBoneSpaces::WorldSpace),
			ManusSkel->GetBoneLocation(ManusSkel->Constraints[i]->ConstraintBone2, EBoneSpaces::WorldSpace), FVector(1, 1, 1));

		//FTransform B2 = ManusSkel->GetBoneTransform(ManusSkel->GetBoneIndex(ManusSkel->Constraints[i]->ConstraintBone2));
		FTransform B1_Component = FTransform(ManusSkel->GetBoneQuaternion(ManusSkel->Constraints[i]->ConstraintBone1, EBoneSpaces::WorldSpace),
			ManusSkel->GetBoneLocation(ManusSkel->Constraints[i]->ConstraintBone1, EBoneSpaces::WorldSpace), FVector(1, 1, 1));

		//FTransform B1Inverse = B1.Inverse();
		FTransform B2_Component_Inverse = B2_Component.Inverse();

		//FTransform B1_T_B2 = B1Inverse * B2;
		FTransform B1_T_B2_Component = B1_Component *  B2_Component_Inverse;

		//FTransform In_Component = W_T_Hand * B1_T_B2;
		

		SkelComp->Constraints[i]->SetAngularDriveParams(OrientationStrengh * 1000, OrientationDamping * 1000, ForceLimit);
		SkelComp->Constraints[i]->SetAngularOrientationTarget(B1_T_B2_Component.GetRotation());
		
		//SkelComp->Constraints[i]->SetAngularOrientationTarget(FQuat(ManusSkel->Constraints[i]->AngularRotationOffset));
	}

	//for (FConstraintInstance* Constraint : SkelComp->Constraints) {
	//	/*if (Constraint->ConstraintBone1.ToString().Contains("Carpal"))
	//	{
	//		Constraint->SetAngularDriveParams(OrientationStrengh, OrientationDamping, ForceLimit);
	//		continue;
	//	}*/
	//	/*if (Constraint->ConstraintBone1.ToString().Contains("Thumb1")) {
	//		SpringActive = SpringIdle * 1000;
	//		Constraint->SetAngularDriveParams(OrientationStrengh * 1000, OrientationDamping * 1000, ForceLimit);
	//		Constraint->SetAngularOrientationTarget(ManusHand->GetBoneQuaternion(Constraint->ConstraintBone1));
	//		continue;
	//	}*/

	//	SpringActive = SpringIdle * 1000;
	//	Constraint->SetAngularDriveParams(OrientationStrengh * 1000, OrientationDamping * 1000, ForceLimit);
	//	FQuat tmp = ManusSkel->GetBoneQuaternion(Constraint->ConstraintBone1);
	//
	//	Constraint->SetAngularOrientationTarget(tmp);
	//	UE_LOG(LogTemp, Warning, TEXT("Mirror Hand: %s"), *SkelComp->GetBoneQuaternion(Constraint->ConstraintBone1, EBoneSpaces::ComponentSpace).ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Manus Hand: %s"), *ManusSkel->GetBoneQuaternion(Constraint->ConstraintBone1, EBoneSpaces::ComponentSpace).ToString());
	//	//UE_LOG(LogTemp, Warning, TEXT("Bone: %s Pose: %s"), *Constraint->ConstraintBone1.ToString(), *ManusSkel->GetBoneQuaternion(Constraint->ConstraintBone1).ToString());
	//}

	



	//FManusGlove GloveData;
	//int64 GloveId;
	//EManusHandType Hand = HandType == EMCGraspAnimHandType::Right ? EManusHandType::Right : EManusHandType::Left;
	//int HandCoefficent = HandType == EMCGraspAnimHandType::Right ? -1 : 1;

	////Getting the current data from the glove, meaning the positon of every finger
	//UManusBlueprintLibrary::GetIdOfFirstAvailableGlove(Hand, GloveId);
	//CoreSdk::GetDataForGlove_UsingGloveId(GloveId, GloveData);

	//// Getting the Position of the tracker
	//FManusTracker Tracker;
	//UManusBlueprintLibrary::GetTrackerData(0, Hand, Tracker);

	//// No Glove Data Recived
	//if (GloveData.Fingers.Num() == 0 || WorldConstraint == nullptr) {
	//	return;
	//}

	//TArray<float> GloveDataArray = {};
	//TArray<float> FingerSpread = {};
	//for (int i = 0; i < 5; i++) {
	//	for (int j = 0; j < 3; j++) {
	//		GloveDataArray.Add(GloveData.Fingers[i].Joints[j].Stretch);
	//	}
	//	FingerSpread.Add(GloveData.Fingers[i].Spread);
	//}
	//int i = 0;
	//int j = 1;
	//for (FConstraintInstance* Constraint : SkelComp->Constraints) {
	//	if (Constraint->ConstraintBone1.ToString().Contains("Carpal"))
	//	{
	//		Constraint->SetAngularDriveParams(OrientationStrengh, OrientationDamping, ForceLimit);
	//		continue;
	//	}
	//	if (Constraint->ConstraintBone1.ToString().Contains("Thumb1")) {
	//		SpringActive = SpringIdle * (GloveDataArray[i] + 1);
	//		Constraint->SetAngularDriveParams(OrientationStrengh * 1000, OrientationDamping * 1000, ForceLimit);
	//		Constraint->SetAngularOrientationTarget(FQuat(0, FingerSpread[0] / 4 * (-1 * HandCoefficent), GloveDataArray[i] * HandCoefficent, 1));
	//		i++;
	//		continue;
	//	}
	//	/*
	//	else if (Constraint->ConstraintBone1.ToString().Contains("1")) {
	//		SpringActive = SpringIdle * (GloveDataArray[i] + 1);
	//		Constraint->SetAngularDriveParams(OrientationStrengh * 1000, OrientationDamping * 1000, ForceLimit);
	//		Constraint->SetAngularOrientationTarget(FQuat(0, FingerSpread[j] * HandCoefficent / 8, GloveDataArray[i] * HandCoefficent, 1));
	//		i++;
	//		j++;
	//		continue;
	//	}*/

	//	// Setting the Constrains for each finger
	//	SpringActive = SpringIdle * (GloveDataArray[i] + 1);
	//	Constraint->SetAngularDriveParams(OrientationStrengh * 1000, OrientationDamping * 1000, ForceLimit);
	//	Constraint->SetAngularOrientationTarget(FQuat(0, 0, GloveDataArray[i] * HandCoefficent, 1));
	//	i++;
	//}


	////const TArray<FManusLiveLinkUser>& ManusLiveLinkUsers = FManusModule::Get().ManusLiveLinkUsers;
	////UE_LOG(LogTemp, Warning, TEXT("%d"), ManusLiveLinkUsers.Num());
	//// Moving the Hand through the world
	//WorldConstraint->SetLinearPositionTarget(Tracker.Transform.GetTranslation());
	//FQuat CorrectedRotation = Tracker.Transform.GetRotation() * FQuat(FRotator(-90, 0, 0));
	//WorldConstraint->SetAngularOrientationTarget(CorrectedRotation.Rotator());

}

// Prepare the skeletal mesh component physics and angular motors
bool UMCGraspAnimController::LoadSkeletalMesh()
{
	if (ASkeletalMeshActor* ParentAsSkMA = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		SkelComp = ParentAsSkMA->GetSkeletalMeshComponent();
		if (SkelComp)
		{
			SkelComp->SetSimulatePhysics(true);
			SkelComp->SetEnableGravity(false);
			UE_LOG(LogTemp, Error, TEXT("==================================================================="));
			if (SkelComp->GetPhysicsAsset())
			{
				//sets up the constraints so they can be moved 
				for (FConstraintInstance* Constraint : SkelComp->Constraints)
				{
					Constraint->SetAngularSwing1Limit(ACM_Free, 0);
					Constraint->SetAngularSwing2Limit(ACM_Free, 0);
					Constraint->SetAngularTwistLimit(ACM_Free, 0);
					Constraint->SetAngularVelocityDriveTwistAndSwing(false, false);
					Constraint->SetAngularVelocityDriveSLERP(true);
					Constraint->SetAngularDriveMode(EAngularDriveMode::SLERP);
					Constraint->SetAngularDriveParams(SpringIdle, Damping, ForceLimit);

				}

				return true;
			}
		}
	}
	return false;
}

// Load the data from the animation data assets in a more optimized form, return true if at least one animation is loaded
bool UMCGraspAnimController::LoadAnimationData()
{
	// Remove any unset references in the array
	AnimationDataAssets.Remove(nullptr);

	for (const auto& AnimationDataAsset : AnimationDataAssets)
	{
		// Iterate frames from the animation
		FAnimation CurrAnim;
		for (const auto& Frame : AnimationDataAsset->Frames)
		{
			// Iterate data from the frame and cache it
			FFrame CurrFrame;
			for (const auto& BoneData : Frame.BonesData)
			{
				if (FConstraintInstance* CI = SkelComp->FindConstraintInstance(FName(*BoneData.Key)))
				{
					CurrFrame.Add(CI, BoneData.Value.AngularOrientationTarget);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find constraint %s"), *FString(__func__), __LINE__, *BoneData.Key);
				}
			}
			CurrAnim.Add(CurrFrame);
		}
		Animations.Add(CurrAnim);
		AnimationNames.Add(AnimationDataAsset->Name);
	}
	return Animations.Num() > 0;
}

// Set the motors target value to the first frame
void UMCGraspAnimController::DriveToFirstFrame()
{
	SpringActive = SpringIdle;
	DriveTarget = ActiveAnimation[0];
	DriveToTarget();
}

// Set the motors target value to the final frame
void UMCGraspAnimController::DriveToLastFrame()
{
	//SpringActive = SpringIdle + (SpringIdle * TriggerStrength);
	const float Strength = bDecreaseStrength ? 1.f / (1.f + TriggerStrength) : 1.f + TriggerStrength;
	SpringActive = SpringIdle * Strength;
	DriveTarget = ActiveAnimation[ActiveAnimation.Num() - 1];
	DriveToTarget();
}

// Bind user inputs for updating the grasps and switching the animations
void UMCGraspAnimController::SetupInputBindings()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->BindAxis(InputAxisName, this, &UMCGraspAnimController::GraspUpdateCallback);
			IC->BindAction(InputNextAnimAction, IE_Pressed, this, &UMCGraspAnimController::GotoNextAnimationCallback);
			IC->BindAction(InputPrevAnimAction, IE_Pressed, this, &UMCGraspAnimController::GotoPreviousAnimationCallback);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d No Input Component found.."), *FString(__func__), __LINE__);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d No Player controller found.."), *FString(__func__), __LINE__);
	}
}

// Compute and set the cached target by interpolating between the two frames
void UMCGraspAnimController::SetTargetUsingLerp(const FFrame& FrameA, const FFrame& FrameB, float Alpha)
{
	for (auto& DT : DriveTarget)
	{
		DT.Value = FMath::LerpRange(FrameA[DT.Key], FrameB[DT.Key], Alpha);
	}
}

// Set the drive parameters to the cached target
void UMCGraspAnimController::DriveToTarget()
{
	for (const auto& DT : DriveTarget)
	{
		DT.Key->SetAngularDriveParams(SpringActive, Damping, ForceLimit);
		DT.Key->SetAngularOrientationTarget(DT.Value.Quaternion());
	}
}

// Calculate the active frame relative to the input value (0 - 1)
int32 UMCGraspAnimController::GetActiveFrameIndex(float Value)
{
	return 0;
}

/* Input callbacks */
// Forward the axis input value to the grasp animation executor
void UMCGraspAnimController::GraspUpdateCallback(float Value)
{
	// If value is almost 1.0, go to the final frame directly
	if (Value > 0.98f)
	{
		if (bIsMax)
		{
			return;
		}
		bIsIdle = false;
		bIsMax = true;
		DriveToLastFrame();
	}
	else if (Value > 0.05f)
	{
		bIsIdle = false;
		bIsMax = false;

		// Increase the spring value proportional with the input
		//SpringActive = SpringIdle + (SpringIdle * TriggerStrength * Value);
		const float Strength = bDecreaseStrength ? 1.f / (1.f + (TriggerStrength * Value)) : 1.f + (TriggerStrength * Value);
		SpringActive = SpringIdle * Strength;

		// Checks in which position we are between the frist frame (0.f) .. () ..  () .. and last frame ((Num()-1).f) 
		float ValueOnTheFrameAxis = Value / ActiveAnimStepSize;

		// Fast floor by casting to in this gives us the nearest smaller frame index
		int32 FrameIndex = static_cast<int32>(ValueOnTheFrameAxis);

		// Gets the value between [0.f,1.f] on how to blend between the frames e.g: 1.66f - 1.f = 0.66f
		float Alpha = ValueOnTheFrameAxis - static_cast<float>(FrameIndex);

		// Set the driver target by interpolating between the nearest smaller frame and the following one
		SetTargetUsingLerp(ActiveAnimation[FrameIndex], ActiveAnimation[FrameIndex + 1], Alpha);
		DriveToTarget();

		//UE_LOG(LogTemp, Warning, TEXT("%s::%d \n\t Value=%f; \n\t ValueOnTheFrameAxis=%f; \n\t FrameIndex=%d; \n\t Alpha=%f; \n\t ActiveAnimStepSize=%f; \n\t Num=%d"),
		//	*FString(__func__), __LINE__, Value, ValueOnTheFrameAxis, FrameIndex, Alpha, ActiveAnimStepSize, ActiveAnimation.Num());
	}
	else if (!bIsIdle)
	{
		bIsIdle = true;
		bIsMax = false;
		DriveToFirstFrame();
	}
}

// Switch to the next grasp animation
void UMCGraspAnimController::GotoNextAnimationCallback()
{
	if (bIsIdle)
	{
		// Increase the index, if it is the last in the array, set it back to 0
		ActiveAnimIdx = ActiveAnimIdx >= Animations.Num() - 1 ? 0 : ActiveAnimIdx + 1;
		ActiveAnimation = Animations[ActiveAnimIdx];
		ActiveAnimStepSize = 1.f / static_cast<float>(ActiveAnimation.Num() - 1);

		DriveToFirstFrame();
		if (bLogDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s::%d Active animation Idx/Name=[%d|%s]"),
				*FString(__func__), __LINE__, ActiveAnimIdx, *AnimationNames[ActiveAnimIdx]);
		}
		//if (HandType == EMCGraspAnimHandType::Left)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("L:%s"),
		//		*AnimationNames[ActiveAnimIdx]), true, FVector2D(1.5f,1.5f));
		//}
		//else
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("R:%s"),
		//		*AnimationNames[ActiveAnimIdx]), true, FVector2D(1.5f, 1.5f));
		//}

		OnGraspType.Broadcast(AnimationNames[ActiveAnimIdx]);
	}
}

// Switch to the previous animation
void UMCGraspAnimController::GotoPreviousAnimationCallback()
{
	if (bIsIdle)
	{
		// Decrease the index, if it becomes smaller than 0, set it to the last the array
		ActiveAnimIdx = ActiveAnimIdx <= 0 ? Animations.Num() - 1 : ActiveAnimIdx - 1;
		ActiveAnimation = Animations[ActiveAnimIdx];
		ActiveAnimStepSize = 1.f / static_cast<float>(ActiveAnimation.Num() - 1);

		DriveToFirstFrame();
		if (bLogDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s::%d Active animation Idx/Name=[%d|%s]"),
				*FString(__func__), __LINE__, ActiveAnimIdx, *AnimationNames[ActiveAnimIdx]);
		}
		//if (HandType == EMCGraspAnimHandType::Left)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("L:%s"),
		//		*AnimationNames[ActiveAnimIdx]), true, FVector2D(1.5f, 1.5f));
		//}
		//else
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("R:%s"),
		//		*AnimationNames[ActiveAnimIdx]), true, FVector2D(1.5f, 1.5f));
		//}

		OnGraspType.Broadcast(AnimationNames[ActiveAnimIdx]);
	}
}
