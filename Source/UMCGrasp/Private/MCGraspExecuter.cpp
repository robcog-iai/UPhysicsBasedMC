// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspExecuter.h"


// Sets default values for this component's properties
UMCGraspExecuter::UMCGraspExecuter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMCGraspExecuter::BeginPlay()
{
	Super::BeginPlay();
	if (ASkeletalMeshActor* TempHand = Cast<ASkeletalMeshActor>(GetOwner()))
	{
		Hand = TempHand;
		USkeletalMeshComponent* const SkelComp = Hand->GetSkeletalMeshComponent();
		SkelComp->SetSimulatePhysics(true);
		SkelComp->SetEnableGravity(false);


		float SpringTemp = 9000.0f;
		float DampingTemp = 1000.0f;
		float ForceLimitTemp = 0.0f;
		if (SkelComp->GetPhysicsAsset())
		{
			// Hand joint velocity drive
			SkelComp->SetAllMotorsAngularPositionDrive(true, true);

			// Set drive parameters
			SkelComp->SetAllMotorsAngularDriveParams(SpringTemp, DampingTemp, ForceLimitTemp);
		}
	}
}


// Called every frame
void UMCGraspExecuter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMCGraspExecuter::UpdateGrasp(const float Input)
{

	if (GraspingData.GetNumberOfEpisodes() <= 1)
	{
		return;
	}

	Spring = 9000 * (1 + Input);

	// Checks if input is relevant
	if (Input > 0.001)
	{
		if (!bIsGrasping) {
			bIsGrasping = true;
		}

		// Calculations for moving in multiple steps
		// StepSize defines how much of the 0-1 input is between each step
		// For 3 Steps for example there is a input of 0,5 seperating each 
		float StepSize = 1 / ((float)GraspingData.GetNumberOfEpisodes() - 1);
		// Calculates how many steps we have passed, given then current input
		// When rounded down we know which step came before with this input
		float StepIteratorCountFloat = Input / StepSize;

		// We will be rounding down and a input of exactly 1 causes problems there
		if (Input >= 1)
		{
			StepIteratorCountFloat = 0.999999 / StepSize;
		}

		// Uses the float to int cast to round down
		int StepIteratorCountInt = (int)StepIteratorCountFloat;

		// We calculate how far the input is past the step that came before it
		float NewInput = StepIteratorCountFloat - (float)StepIteratorCountInt;

		// Manipulate Orientation Drives
		FMCEpisodeData TargetOrientation;
		LerpHandOrientation(&TargetOrientation, GraspingData.GetPositionDataWithIndex(StepIteratorCountInt), GraspingData.GetPositionDataWithIndex(StepIteratorCountInt + 1), NewInput);
		DriveToHandOrientationTarget(&TargetOrientation);

	}
	else {
		if (bIsGrasping)
		{
			// Stop grasping
			StopGrasping();
		}
	}
}

void UMCGraspExecuter::StopGrasping()
{
	// Stop Grasp
	FMCEpisodeData Save = GraspingData.GetPositionDataWithIndex(0);
	DriveToHandOrientationTarget(&Save);
	if (bIsInQueue)
	{
		GraspingData = GraspQueue;
		bIsInQueue = false;
	}
	bIsGrasping = false;
}

void UMCGraspExecuter::LerpHandOrientation(FMCEpisodeData* Target, FMCEpisodeData Initial, FMCEpisodeData Closed, const float Input)
{
	TArray<FString> TempArray;
	Initial.GetMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		Target->AddNewBoneData(s, FMath::LerpRange(
			Initial.GetBoneData(s)->AngularDriveInput,
			Closed.GetBoneData(s)->AngularDriveInput, Input));
	}
}

void UMCGraspExecuter::DriveToHandOrientationTarget(FMCEpisodeData* Target)
{
	FConstraintInstance* Constraint = nullptr;
	TArray<FString> TempArray;
	Target->GetMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		Constraint = BoneNameToConstraint(s);
		if (Constraint) {
			Constraint->SetAngularOrientationTarget(Target->GetMap()->Find(s)->AngularDriveInput.Quaternion());
		}
	}
}

FConstraintInstance* UMCGraspExecuter::BoneNameToConstraint(FString BoneName) {
	FConstraintInstance* Constraint = nullptr;
	UActorComponent* component = Hand->GetComponentByClass(USkeletalMeshComponent::StaticClass());
	USkeletalMeshComponent* skeletalComponent = Cast<USkeletalMeshComponent>(component);

	//sets up the constraints so they can be moved 
	if (!bBonesConstraintsSetUp) {
		for (FConstraintInstance* NewConstraint : skeletalComponent->Constraints) {
			if (NewConstraint->ConstraintBone1.ToString() == BoneName) {
				NewConstraint->SetOrientationDriveTwistAndSwing(false, false);
				NewConstraint->SetAngularVelocityDriveTwistAndSwing(false, false);
				NewConstraint->SetAngularVelocityDriveSLERP(false);
				NewConstraint->SetOrientationDriveSLERP(true);
				NewConstraint->SetAngularDriveParams(Spring, 1000, 0);
			}
		}
		bBonesConstraintsSetUp = true;
	}

	//finds the constraint responsible for moving this bone
	for (FConstraintInstance* NewConstraint : skeletalComponent->Constraints) 
	{
		NewConstraint->SetAngularDriveParams(Spring, 1000, 0);
		if (NewConstraint->ConstraintBone1.ToString() == BoneName) {
			Constraint = NewConstraint;
		}
	}
	return Constraint;
}

void UMCGraspExecuter::SetGraspingData(FMCAnimationData Data) {
	// if player is grasping put new grasp in queue, else change grasp immediately
	if (bIsGrasping)
	{
		GraspQueue = Data;
		bIsInQueue = true;
		return;
	}
	GraspingData = Data;
}
