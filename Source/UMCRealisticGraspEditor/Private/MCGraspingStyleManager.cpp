// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspingStyleManager.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "MCAnimationDataStructure.h"
#include "MCWrite.h"
#include "PersonaModule.h"
#include "IPersonaPreviewScene.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"

AAMCGraspingStyleManager::AAMCGraspingStyleManager() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAMCGraspingStyleManager::BeginPlay()
{

}

void AAMCGraspingStyleManager::Tick(float DeltaTime)
{
	//Do not do anything if the mesjh is not set.
	if (SelectedMesh == nullptr) {
		return;
	}

	// Create a step out of the bones rotations and names
	// if it is the first tick or if CurrentTime is greater than StepSize.
	if (TotalTime == 0 || !bReachedEndTime && CurrentTime >= StepSize) {

		//Get the USkeletalMeshComponent to access the mesh and its bones.
		UActorComponent* Component = SelectedMesh->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		USkeletalMeshComponent*  SkeletalComponent = Cast<USkeletalMeshComponent>(Component);
		//Get the bone data as map with the name of the bone as FString and the rotation of the bone
		 GetBoneDataForStep(SkeletalComponent);

		//Initialize an AnimationData which contains all the necessary information for a grasp.
		if (!bHasSendFirstData) {
			CreateAnimationData(CalculatedBoneRotations);
			bHasSendFirstData = true;
		}

		AnimationData.AddNewEpisode(NewEpisodeData);

		CurrentTime = 0;
	}

	//Save the rotations of the bones when the TotalTime is greater than the EndTime of the passed animation.
	if (!bReachedEndTime && TotalTime > EndTime - 0.1) {
		bReachedEndTime = true;
		UActorComponent* Component = SelectedMesh->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		USkeletalMeshComponent*  SkeletalComponent = Cast<USkeletalMeshComponent>(Component);
		GetBoneDataForStep(SkeletalComponent);
		if (!bHasSendFirstData) {
			CreateAnimationData(CalculatedBoneRotations);
			bHasSendFirstData = true;
		}
		AnimationData.AddNewEpisode(NewEpisodeData);

		//Write the AnimationData into a .ini file.
		UMCWrite::WriteFile(AnimationData);
	}
	CurrentTime += DeltaTime;
	TotalTime += DeltaTime;
}

void AAMCGraspingStyleManager::GetBoneDataForStep(USkeletalMeshComponent * SkeletalComponent)
{
	NewEpisodeData = TMap<FString, FMCBoneData>();

	TArray<FTransform> BoneSpaceTransforms = SkeletalComponent->BoneSpaceTransforms;
	TArray<FName> BoneNames;
	SkeletalComponent->GetBoneNames(BoneNames);
	TMap<FString, FRotator> CurrentBoneSpaceTransforms;
	int Index = 0;
	for (FTransform BoneTransform : BoneSpaceTransforms)
	{
		CurrentBoneSpaceTransforms.Add(BoneNames[Index].ToString(), BoneTransform.GetRotation().Rotator());
		Index++;
	}
	//go over each constraint in the skeleton. bone1 is the one affected by it 
	for (FConstraintInstance* NewConstraint : SkeletalComponent->Constraints)
	{
		FMCBoneData NewBoneData = FMCBoneData();
		//get the rotation of bones
		FQuat QuatBone1 = SkeletalComponent->GetBoneQuaternion(NewConstraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
		FQuat QuatBone2 = SkeletalComponent->GetBoneQuaternion(NewConstraint->ConstraintBone2, EBoneSpaces::ComponentSpace);
		FRotator *FoundRotation = CurrentBoneSpaceTransforms.Find(NewConstraint->ConstraintBone1.ToString());
		NewBoneData.BoneSpace = *FoundRotation;

		//save the start rotations of all the bones
		if (!StartRotatorsSet)
		{
			StartRotators.Add(NewConstraint->ConstraintBone1.ToString(), QuatBone1);
			StartRotators.Add(NewConstraint->ConstraintBone2.ToString(), QuatBone2);
			NewBoneData.AngularDriveInput = FRotator(0, 0, 0);
			CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), FRotator(0, 0, 0));
		}
		else
		{
			//calculate how much bone1's roation has changed relative to the start 
			FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(*StartRotators.Find(NewConstraint->ConstraintBone1.ToString())));
			FQuat Quat1Difference = Transform1Difference.GetRotation();
			FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(*StartRotators.Find(NewConstraint->ConstraintBone2.ToString())));
			FQuat Quat2Difference = Transform2Difference.GetRotation();
			//substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
			FQuat Quat = Quat1Difference * Quat2Difference.Inverse();
			NewBoneData.AngularDriveInput = Quat.Rotator();
			CalculatedBoneRotations.Add(NewConstraint->ConstraintBone1.ToString(), (Quat.Rotator()));
		}
		NewEpisodeData.Add(NewConstraint->ConstraintBone1.ToString(), NewBoneData);
	}


	if (!StartRotatorsSet)
	{
		StartRotatorsSet = true;
	}
}

void AAMCGraspingStyleManager::CreateAnimationData(TMap<FString, FRotator> StepData)
{
	AnimationData.AnimationName = GraspingStyleName;
	AnimationData.BoneNames = TArray<FString>();
	for (auto& Elem : StepData)
	{
		AnimationData.BoneNames.Add(Elem.Key);
	}
}

